#ifndef YAHTSEE_CONNECTION_STATE_H
#define YAHTSEE_CONNECTION_STATE_H

#include <nlohmann/json.hpp>

namespace yahtsee
{
    class GameLogic;
    class Connection;

    class ConnectionState
    {
        public:
            typedef nlohmann::json Packet;

            ConnectionState(GameLogic *game);

            void handle(Connection *conn, const Packet &packet);

        private:

            void handle_player_roll(Connection *conn, const Packet &);

            void handle_game_start(Connection *conn, const Packet &);

            void handle_connection_init(Connection *conn, const Packet &);

            void handle_remote_connection_init(Connection *conn, const Packet &);

            void handle_player_joined(Connection *conn, const Packet &);

            void handle_player_left(Connection *conn, const Packet &);

            void handle_player_turn_finished(Connection *conn, const Packet &);

            GameLogic *game_;
    };

}


#endif


