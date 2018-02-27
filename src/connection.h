//
// Created by Ryan Jennings on 2018-02-27.
//

#ifndef YAHTSEE_CONNECTION_H
#define YAHTSEE_CONNECTION_H

namespace yahtsee {

    class Player;

    /*!
     * A connection is a remote user connected to this host instance
     */
    class Connection : public rj::net::buffered_socket {
    public:
        typedef nlohmann::json Packet;

        Connection(rj::net::SOCKET sock, const sockaddr_storage &addr);

        Connection();

        /* non copyable */
        Connection(const Connection &other) = delete;

        Connection(Connection &&other);

        virtual ~Connection();

        /* non copyable */
        Connection &operator=(const Connection &other) = delete;

        Connection &operator=(Connection &&other);

        virtual void on_will_read();

        virtual void on_did_read();

        virtual void on_will_write();

        virtual void on_did_write();

        virtual void on_connect(const std::vector<Player> &players);

        virtual void on_close();

    protected:
        void handle_player_roll(const Packet &);

        void handle_game_start(const Packet &);

        void handle_connection_init(const Packet &);

        void handle_remote_connection_init(const Packet &);

        void handle_player_joined(const Packet &);

        void handle_player_left(const Packet &);

        void handle_player_turn_finished(const Packet &);
    };

}

#endif //YAHTSEE_CONNECTION_H
