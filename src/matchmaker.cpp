#include <nlohmann/json.hpp>
#include "game.h"
#include "matchmaker.h"
#include "player.h"
#include "connection.h"
#include "connection_factory.h"
#include <coda/log/log.h>

#ifdef UPNPC
#include <miniupnpc.h>
#include <upnpcommands.h>
#endif

using namespace coda;

namespace yahtsee {

#ifndef DEBUG
    const char *Matchmaker::GAME_API_URL = "connect.micrantha.com";
#else
    const char *Matchmaker::GAME_API_URL = "localhost.micrantha.com:1337";
#endif

    const char *Matchmaker::GAME_TYPE = "yahtsee";

    void Matchmaker::send_network_message(const string &value) {
        if (server_.is_valid()) {
            clientFactory_->for_connections([&value](const shared_ptr<Connection> &conn) {
                conn->writeln(value);
            });
        }

        if (client_.is_valid()) {
            client_.writeln(value);
        }
    }

    void Matchmaker::set_api_keys(const string &appId, const string &appToken) {
        api_.add_header("X-Application-Id", appId);

        api_.add_header("X-Application-Token", appToken);
    }

    Matchmaker::Matchmaker(StateManager *state)
            : api_(GAME_API_URL),
              client_(),
              clientFactory_(std::make_shared<ConnectionFactory>(state)),
              server_(clientFactory_) {

        api_.add_header("Content-Type", "application/json; charset=UTF-8");

        api_.add_header("Accept", "application/json, */*");
    }

    Matchmaker::Matchmaker(Matchmaker &&other)
            : gameId_(std::move(other.gameId_)),
              api_(std::move(other.api_)),
              client_(std::move(other.client_)),
              clientFactory_(std::move(other.clientFactory_)),
              server_(std::move(other.server_)) {
    }

    Matchmaker &Matchmaker::operator=(Matchmaker &&other) {
        server_ = std::move(other.server_);
        client_ = std::move(other.client_);
        gameId_ = std::move(other.gameId_);
        api_ = std::move(other.api_);
        clientFactory_ = std::move(other.clientFactory_);

        return *this;
    }

    Matchmaker::~Matchmaker() {
        stop();
    }

    void Matchmaker::stop() {
        server_.stop();

        client_.close();

        unregister_with_service();
    }

    bool Matchmaker::join_best_game(string *error) {

        /* call the api for the best available game */
        Packet packet;

        packet["type"] = GAME_TYPE;

        api_.set_content(packet).post("api/v1/games/best");

        int response = api_.response().code();

        /* handle an error */
        if (response != net::http::OK) {
            if (error) {
                packet = Packet::parse(api_.response().content());
                *error = packet["error"];
            }
            return false;
        }

        /* parse the response */

        packet = Packet::parse(api_.response().content());

        string ip = packet["host"];

        int port = packet["port"];

        /* start the client */
        return join_game(ip, port, error);
    }

    bool Matchmaker::join_game(const std::string &host, int port, string *error) {
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

        log::trace("joining game");
        return rval;
    }

    void Matchmaker::port_forward(int port) const {
#ifdef UPNPC
        int error = 0;

        // get a list of upnp devices (asks on the broadcast address and returns the responses)
        struct UPNPDev *upnp_dev = upnpDiscover(1000,     // timeout in milliseconds
                                                NULL,     // multicast address, default = "239.255.255.250"
                                                NULL,     // minissdpd socket, default = "/var/run/minissdpd.sock"
                                                0,        // source port, default = 1900
                                                0,        // 0 = IPv4, 1 = IPv6
                                                &error);  // error output

        if (upnp_dev == NULL || error != 0) {
            log::trace("Could not discover upnp device");
            freeUPNPDevlist(upnp_dev);
            return;
        }

        char lan_address[INET6_ADDRSTRLEN];  // maximum length of an ipv6 address string
        struct UPNPUrls upnp_urls;
        struct IGDdatas upnp_data;
        int status = UPNP_GetValidIGD(upnp_dev, &upnp_urls, &upnp_data, lan_address, sizeof(lan_address));

        if (status != 1)  // there are more status codes in minupnpc.c but 1 is success all others are different failures
        {
            log::trace("No valid Internet Gateway Device could be connected to");
            FreeUPNPUrls(&upnp_urls);
            freeUPNPDevlist(upnp_dev);
            return;
        }

        // get the external (WAN) IP address
        char wan_address[INET6_ADDRSTRLEN];

        if (UPNP_GetExternalIPAddress(upnp_urls.controlURL, upnp_data.first.servicetype, wan_address) != 0) {
            log::trace("Could not get external IP address");
        } else {
            log::trace("External IP: %s", wan_address);
        }

        // add a new TCP port mapping from WAN port 12345 to local host port 24680
        error = UPNP_AddPortMapping(
            upnp_urls.controlURL, upnp_data.first.servicetype,
            std::to_string(port).c_str(),  // external (WAN) port requested
            std::to_string(port).c_str(),  // internal (LAN) port to which packets will be redirected
            lan_address,                   // internal (LAN) address to which packets will be redirected
            "Yahtsee Server",              // text description to indicate why or who is responsible for the port mapping
            "TCP",                         // protocol must be either TCP or UDP
            NULL,                          // remote (peer) host address or nullptr for no restriction
            "86400");                      // port map lease duration (in seconds) or zero for "as long as possible"

        if (error) {
            log::trace("Failed to map ports\n");
        } else {
            log::trace("Successfully mapped ports");
        }

        log::trace("Lan Address\tWAN Port -> LAN Port\tProtocol\tDuration\tEnabled?\tRemote Host\tDescription\n");
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
            error = UPNP_GetGenericPortMappingEntry(
                upnp_urls.controlURL, upnp_data.first.servicetype, indexStr, map_wan_port, map_lan_address, map_lan_port,
                map_protocol, map_description, map_mapping_enabled, map_remote_host, map_lease_duration);

            if (error) {
                break;  // no more port mappings available
            }

            log::trace("%s\t%s -> %s\t%s\t%s\t%s\t%s\t%s", map_lan_address, map_wan_port, map_lan_port, map_protocol,
                      map_lease_duration, map_mapping_enabled, map_remote_host, map_description);
        }

        FreeUPNPUrls(&upnp_urls);
        freeUPNPDevlist(upnp_dev);
#endif
    }

    bool Matchmaker::host(const Config &settings, bool registerOnline, bool portForwarding, string *error, int port) {
        if (port == INVALID) {
            log::trace("no port specified, randomizing.");
            port = (rand() % 65535) + 1024;
        }

        if (registerOnline) {
            if (!register_with_service(settings, error, port)) {
                return false;
            }
        }

        if (portForwarding) {
            port_forward(port);
        }

        serverPort_ = port;

        server_.start_in_background(port);

        return true;
    }

    bool Matchmaker::register_with_service(const Config &settings, string *error, int port) {
        Packet packet;

        packet["type"] = GAME_TYPE;

        packet["port"] = port;

        if (settings.count("connect_service")) {
            auto obj = settings.at("connect_service");

            if (obj.count("timeout")) {
                long timeout = obj["timeout"].get<long>() * 1000;
                log::trace("setting timeout to %ld", timeout);
                api_.set_timeout(timeout);
            }
        }

        api_.set_content(packet).post("api/v1/games/register");

        int response = api_.response().code();

        if (response != coda::net::http::OK) {
            if (error) {
                packet = Packet::parse(api_.response().content());
                *error = packet["error"];
            }

            return false;
        }

        gameId_ = api_.response();

        log::trace("hosting game %s", gameId_.c_str());

        return true;
    }

    int Matchmaker::port() const {
        return serverPort_;
    }

    void Matchmaker::unregister_with_service() {
        if (!gameId_.empty()) {
            api_.set_content(gameId_).post("api/v1/games/unregister");

            if (api_.response().code() != net::http::OK) {
                log::trace("Unable to unregister game");
            } else {
                log::trace("game %s unregistered", gameId_.c_str());
            }

            gameId_.clear();
        }
    }

    void Matchmaker::notify_game_start(const std::shared_ptr<Player> &player) {
        Packet packet;

        packet["action"] = static_cast<int>(ClientAction::START);

        packet["start_id"] = player->id();

        send_network_message(packet);

        log::trace("notify game started %s", packet.dump().c_str());

        unregister_with_service();
    }

    void Matchmaker::notify_player_joined(const shared_ptr<Player> &player) {
        Packet packet;

        packet["action"] = static_cast<int>(ClientAction::PLAYER_JOINED);

        packet["player"] = player->to_packet();

        send_network_message(packet);

        log::trace("notify player joined %s", packet.dump().c_str());
    }

    void Matchmaker::notify_player_left(const shared_ptr<Player> &player) {
        Packet packet;

        packet["action"] = static_cast<int>(ClientAction::PLAYER_LEFT);

        packet["player"] = player->to_packet();

        send_network_message(packet);

        log::trace("notify player left %s", packet.dump().c_str());
    }

    void Matchmaker::notify_player_roll(const std::shared_ptr<Player> &player) {
        Packet packet;

        packet["action"] = static_cast<int>(ClientAction::PLAYER_ROLL);

        auto dice = player->d1ce();

        vector<Packet> values;

        for (size_t i = 0; i < player->die_count(); i++) {
            Packet inner;

            inner["kept"] = player->is_kept(i);
            inner["value"] = player->d1e(i).value();

            values.push_back(inner);
        }

        packet["player_id"] = player->id();

        packet["roll"] = values;

        send_network_message(packet);

        log::trace("notify player roll %s", packet.dump().c_str());
    }

    void Matchmaker::notify_player_turn_finished(const std::shared_ptr<Player> &player) {
        Packet packet;

        vector<Packet> upper, lower;

        packet["action"] = static_cast<int>(ClientAction::PLAYER_TURN_FINISHED);

        for (int i = 0; i <= yaht::Constants::NUM_DICE; i++) {
            if (!player->score().upper_played(i + 1)) {
                upper.push_back(-1);
            } else {
                auto value = player->score().upper_score(i + 1);

                upper.push_back(value);
            }
        }

        packet["upper"] = upper;

        for (int i = yaht::scoresheet::FIRST_TYPE; i < yaht::scoresheet::MAX_TYPE; i++) {
            yaht::scoresheet::type type = static_cast<yaht::scoresheet::type>(i);

            if (!player->score().lower_played(type)) {
                lower.push_back(-1);
            } else {
                auto value = player->score().lower_score(type);

                lower.push_back(value);
            }
        }

        packet["lower"] = lower;

        packet["player_id"] = player->id();

        send_network_message(packet);

        log::trace("notify player turn finished %s", packet.dump().c_str());
    }

}