#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "matchmaker.h"
#include <arg3json/json.h>
#include "game.h"
#include "player.h"
#include "log.h"

#ifdef HAVE_LIBMINIUPNPC
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#elif MINIUPNPC_FOUND
#include <miniupnpc.h>
#include <upnpcommands.h>
#endif

using namespace arg3;

#ifndef DEBUG
const char *matchmaker::GAME_API_URL = "connect.arg3.com";
#else
const char *matchmaker::GAME_API_URL = "localhost.arg3.com:1337";
#endif

void matchmaker::send_network_message(const string &value)
{
    if (server_.is_valid()) {
        client_factory_.for_connections([&value](const shared_ptr<connection> &conn) { conn->writeln(value); });
    }

    if (client_.is_valid()) {
        client_.writeln(value);
    }
}

void matchmaker::set_api_keys(const string &appId, const string &appToken)
{
    api_.add_header("X-Application-Id", appId);

    api_.add_header("X-Application-Token", appToken);
}

matchmaker::matchmaker(game *game) : api_(GAME_API_URL), client_(game), client_factory_(game), server_(&client_factory_), game_(game)
{
    api_.add_header("Content-Type", "application/json; charset=UTF-8");

    api_.add_header("Accept", "application/json, */*");
}

matchmaker::matchmaker(matchmaker &&other)
    : gameId_(std::move(other.gameId_)),
      api_(std::move(other.api_)),
      client_(std::move(other.client_)),
      client_factory_(std::move(other.client_factory_)),
      server_(std::move(other.server_)),
      game_(other.game_)
{
}

matchmaker &matchmaker::operator=(matchmaker &&other)
{
    server_ = std::move(other.server_);
    client_ = std::move(other.client_);
    gameId_ = std::move(other.gameId_);
    api_ = std::move(other.api_);
    client_factory_ = std::move(other.client_factory_);
    game_ = other.game_;

    return *this;
}

matchmaker::~matchmaker()
{
    stop();
}
void matchmaker::stop()
{
    server_.stop();

    client_.close();

    unregister();
}

bool matchmaker::is_valid() const
{
    return game_ && game_->is_online();
}
bool matchmaker::join_best_game(string *error)
{
    if (!is_valid()) {
        *error = "Not online.";
        return false;
    }

    /* call the api for the best available game */
    json::object type;

    type.set_string("type", GAME_TYPE);

    api_.set_payload(type.to_string()).post("api/v1/games/best");

    int response = api_.response().code();

    /* handle an error */
    if (response != net::http::OK) {
        if (error) {
            json::object json;
            json.parse(api_.response());
            *error = json.get_string("error");
        }

        return false;
    }

    /* parse the response */
    json::object game;

    game.parse(api_.response());

    string ip = game.get_string("host");

    int port = game.get_int("port");

    /* start the client */
    return join_game(ip, port, error);
}

bool matchmaker::join_game(const std::string &host, int port, string *error)
{
    if (!is_valid()) {
        *error = "Not online.";
        return false;
    }

    /* start the client */
    bool rval = client_.start_in_background(host, port);

    /* handle an error */
    if (!rval) {
        char buf[BUFSIZ + 1] = {0};
        snprintf(buf, BUFSIZ, "Unable to connect to %s:%d", host.c_str(), port);
        if (error) {
            *error = buf;
        }
    }

    log_trace("joining game");
    return rval;
}

void matchmaker::port_forward(int port) const
{
#ifdef HAVE_LIBMINIUPNPC
    int error = 0;

    // get a list of upnp devices (asks on the broadcast address and returns the responses)
    struct UPNPDev *upnp_dev = upnpDiscover(1000,     // timeout in milliseconds
                                            NULL,     // multicast address, default = "239.255.255.250"
                                            NULL,     // minissdpd socket, default = "/var/run/minissdpd.sock"
                                            0,        // source port, default = 1900
                                            0,        // 0 = IPv4, 1 = IPv6
                                            &error);  // error output

    if (upnp_dev == NULL || error != 0) {
        log_trace("Could not discover upnp device");
        freeUPNPDevlist(upnp_dev);
        return;
    }

    char lan_address[INET6_ADDRSTRLEN];  // maximum length of an ipv6 address string
    struct UPNPUrls upnp_urls;
    struct IGDdatas upnp_data;
    int status = UPNP_GetValidIGD(upnp_dev, &upnp_urls, &upnp_data, lan_address, sizeof(lan_address));

    if (status != 1)  // there are more status codes in minupnpc.c but 1 is success all others are different failures
    {
        log_trace("No valid Internet Gateway Device could be connected to");
        FreeUPNPUrls(&upnp_urls);
        freeUPNPDevlist(upnp_dev);
        return;
    }

    // get the external (WAN) IP address
    char wan_address[INET6_ADDRSTRLEN];

    if (UPNP_GetExternalIPAddress(upnp_urls.controlURL, upnp_data.first.servicetype, wan_address) != 0) {
        log_trace("Could not get external IP address");
    } else {
        log_trace("External IP: %s", wan_address);
    }

    // add a new TCP port mapping from WAN port 12345 to local host port 24680
    error = UPNP_AddPortMapping(upnp_urls.controlURL, upnp_data.first.servicetype,
                                std::to_string(port).c_str(),  // external (WAN) port requested
                                std::to_string(port).c_str(),  // internal (LAN) port to which packets will be redirected
                                lan_address,                   // internal (LAN) address to which packets will be redirected
                                "Yahtsee Server",              // text description to indicate why or who is responsible for the port mapping
                                "TCP",                         // protocol must be either TCP or UDP
                                NULL,                          // remote (peer) host address or nullptr for no restriction
                                "86400");                      // port map lease duration (in seconds) or zero for "as long as possible"

    if (error) {
        log_trace("Failed to map ports\n");
    } else {
        log_trace("Successfully mapped ports");
    }

    log_trace("Lan Address\tWAN Port -> LAN Port\tProtocol\tDuration\tEnabled?\tRemote Host\tDescription\n");
    // list all port mappings
    for (size_t index = 0;; ++index) {
        char map_wan_port[6] = "";
        char map_lan_address[16] = "";
        char map_lan_port[6] = "";
        char map_protocol[4] = "";
        char map_description[80] = "";
        char map_mapping_enabled[4] = "";
        char map_remote_host[64] = "";
        char map_lease_duration[16] = "";  // original time, not remaining time :(

        char indexStr[10];
        sprintf(indexStr, "%zu", index);
        error =
            UPNP_GetGenericPortMappingEntry(upnp_urls.controlURL, upnp_data.first.servicetype, indexStr, map_wan_port, map_lan_address, map_lan_port,
                                            map_protocol, map_description, map_mapping_enabled, map_remote_host, map_lease_duration);

        if (error) {
            break;  // no more port mappings available
        }

        log_trace("%s\t%s -> %s\t%s\t%s\t%s\t%s\t%s", map_lan_address, map_wan_port, map_lan_port, map_protocol, map_lease_duration,
                  map_mapping_enabled, map_remote_host, map_description);
    }

    FreeUPNPUrls(&upnp_urls);
    freeUPNPDevlist(upnp_dev);

#endif
}

bool matchmaker::host(bool register_online, bool port_forwarding, string *error, int port)
{
    if (!is_valid()) {
        *error = "Not online.";
        return false;
    }

    if (port == INVALID) {
        log_trace("no port specified, randomizing.");
        port = (rand() % 65535) + 1024;
    }

    if (register_online) {
        if (!r3gister(error, port)) {
            return false;
        }
    }

    if (port_forwarding) {
        port_forward(port);
    }

    server_port_ = port;

    server_.start_in_background(port);

    return true;
}

bool matchmaker::r3gister(string *error, int port)
{
    json::object json;

    auto settings = game_->settings();

    json.set_string("type", GAME_TYPE);

    json.set_int("port", port);

    if (settings->contains("arg3connect")) {
        auto obj = settings->get("arg3connect");

        if (obj.contains("timeout")) {
            long timeout = obj.get_int("timeout") * 1000;
            log_trace("setting timeout to %ld", timeout);
            api_.set_timeout(timeout);
        }
    }

    api_.set_payload(json.to_string()).post("api/v1/games/register");

    int response = api_.response().code();

    if (response != arg3::net::http::OK) {
        if (error) {
            json::object json;
            json.parse(api_.response());
            *error = json.get_string("error");
        }

        return false;
    }

    gameId_ = api_.response();

    log_trace("hosting game %s", gameId_.c_str());

    return true;
}

int matchmaker::server_port() const
{
    return server_port_;
}
void matchmaker::unregister()
{
    if (!gameId_.empty()) {
        api_.set_payload(gameId_).post("api/v1/games/unregister");

        if (api_.response().code() != net::http::OK) {
            log_trace("Unable to unregister game");
        } else {
            log_trace("game %s unregistered", gameId_.c_str());
        }

        gameId_.clear();
    }
}

void matchmaker::notify_game_start()
{
    if (!is_valid()) {
        return;
    }

    json::object json;

    json.set_int("action", GAME_START);

    json.set_string("start_id", game_->current_player()->id());

    send_network_message(json.to_string());

    log_trace("notify game started %s", json.to_string().c_str());

    unregister();
}

void matchmaker::notify_player_joined(const shared_ptr<player> &p)
{
    if (!is_valid()) {
        return;
    }

    json::object json;

    json.set_int("action", PLAYER_JOINED);

    json.set("player", p->to_json());

    send_network_message(json.to_string());

    log_trace("notify player joined %s", json.to_string().c_str());
}

void matchmaker::notify_player_left(const shared_ptr<player> &p)
{
    if (!is_valid()) {
        return;
    }

    json::object json;

    json.set_int("action", PLAYER_LEFT);

    json.set("player", p->to_json());

    send_network_message(json.to_string());

    log_trace("notify player left %s", json.to_string().c_str());
}

void matchmaker::notify_player_roll()
{
    if (!is_valid()) {
        return;
    }

    json::object json;

    json.set_int("action", PLAYER_ROLL);

    auto dice = game_->current_player()->d1ce();

    auto player = game_->current_player();

    json::array values;

    for (size_t i = 0; i < player->die_count(); i++) {
        json::object inner;

        inner.set_bool("kept", player->is_kept(i));
        inner.set_int("value", player->d1e(i).value());

        values.add(inner);
    }

    json.set_string("player_id", player->id());

    json.set_array("roll", values);

    send_network_message(json.to_string());

    log_trace("notify player roll %s", json.to_string().c_str());
}

void matchmaker::notify_player_turn_finished()
{
    if (!is_valid()) {
        return;
    }

    json::object json;

    json::array upper, lower;

    json.set_int("action", PLAYER_TURN_FINISHED);

    auto player = game_->current_player();

    for (int i = 0; i <= yaht::Constants::NUM_DICE; i++) {
        if (!player->score().upper_played(i + 1)) {
            upper.add_int(-1);
        } else {
            auto value = player->score().upper_score(i + 1);

            upper.add_int(value);
        }
    }

    json.set_array("upper", upper);

    for (int i = yaht::scoresheet::FIRST_TYPE; i < yaht::scoresheet::MAX_TYPE; i++) {
        yaht::scoresheet::type type = static_cast<yaht::scoresheet::type>(i);

        if (!player->score().lower_played(type)) {
            lower.add_int(-1);
        } else {
            auto value = player->score().lower_score(type);

            lower.add_int(value);
        }
    }

    json.set_array("lower", lower);

    json.set_string("player_id", player->id());

    send_network_message(json.to_string());

    log_trace("notify player turn finished %s", json.to_string().c_str());
}
