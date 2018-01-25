#ifndef _MATCHMAKER_H_
#define _MATCHMAKER_H_

#include <rj/dice/yaht/player.h>
#include <rj/net/socket_server.h>
#include <rj/net/uri.h>
#include <rj/net/http/client.h>
#include <rj/net/async/server.h>
#include <string>
#include "client.h"

class game;
class player;

class matchmaker
{
   public:
    typedef nlohmann::json packet_format;
    matchmaker(game *game);
    matchmaker(const matchmaker &) = delete;
    matchmaker(matchmaker &&other);
    matchmaker &operator=(const matchmaker &) = delete;
    matchmaker &operator=(matchmaker &&);
    virtual ~matchmaker();
    void stop();
    bool is_valid() const;
    bool host(bool register_online, bool port_forwarding, std::string *error = NULL, int port = INVALID);
    bool join_best_game(std::string *error = NULL);
    bool join_game(const std::string &host, int port, std::string *error = NULL);
    void notify_player_joined(const shared_ptr<player> &p);
    void notify_player_left(const shared_ptr<player> &p);
    void notify_game_start();
    void notify_player_roll();
    void notify_player_turn_finished();
    int server_port() const;
    void set_api_keys(const std::string &appId, const std::string &appToken);

   private:
    bool r3gister(std::string *error, int port);

    void unregister();

    void port_forward(int port) const;

    void send_network_message(const string &value);

    static const char *GAME_TYPE;
    static const char *GAME_API_URL;
    static const int INVALID = -1;

    std::string gameId_;
    rj::net::http::client api_;
    client client_;
    std::shared_ptr<connection_factory> client_factory_;
    rj::net::async::server server_;
    game *game_;
    int server_port_;
};

#endif
