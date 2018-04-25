#ifndef YAHTSEE_MATCHMAKER_H
#define YAHTSEE_MATCHMAKER_H

#include <coda/dice/yaht/player.h>
#include <coda/net/socket_server.h>
#include <coda/net/uri.h>
#include <coda/net/http/client.h>
#include <coda/net/async/server.h>
#include <string>
#include "client.h"
#include "connection_factory.h"

namespace yahtsee {

    class StateManager;
    class ConnectionFactory;
    class Player;

    class Matchmaker {
    public:
        typedef nlohmann::json Packet;

        typedef nlohmann::json Config;

        // default constructor
        Matchmaker(StateManager *state);

        // non-copy
        Matchmaker(const Matchmaker &) = delete;

        // movable
        Matchmaker(Matchmaker &&other);

        // non-assignable
        Matchmaker &operator=(const Matchmaker &) = delete;

        // move assignable
        Matchmaker &operator=(Matchmaker &&);

        ~Matchmaker();

        void stop();

        bool host(const Config &settings, bool registerOnline, bool portForwarding, std::string *error = NULL, int port = INVALID);

        bool join_best_game(std::string *error = NULL);

        bool join_game(const std::string &host, int port, std::string *error = NULL);

        void notify_player_joined(const shared_ptr<Player> &p);

        void notify_player_left(const shared_ptr<Player> &p);

        void notify_game_start(const shared_ptr<Player> &p);

        void notify_player_roll(const shared_ptr<Player> &p);

        void notify_player_turn_finished(const shared_ptr<Player> &p);

        int port() const;

        void set_api_keys(const std::string &appId, const std::string &appToken);

    private:
        bool register_with_service(const Config &settings, std::string *error, int port);

        void unregister_with_service();

        void port_forward(int port) const;

        void send_network_message(const string &value);

        static const char *GAME_TYPE;
        static const char *GAME_API_URL;
        static const int INVALID = -1;

        std::string gameId_;
        coda::net::http::client api_;
        Client client_;
        std::shared_ptr<ConnectionFactory> clientFactory_;
        coda::net::async::server server_;
        int serverPort_;
    };

}
#endif
