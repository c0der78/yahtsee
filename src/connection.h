//
// Created by Ryan Jennings on 2018-02-27.
//

#ifndef YAHTSEE_CONNECTION_H
#define YAHTSEE_CONNECTION_H

#include <nlohmann/json.hpp>
#include <rj/net/buffered_socket.h>

namespace yahtsee {

    class Player;
    class ConnectionState;

    /*!
     * A connection is a remote user connected to this host instance
     */
    class Connection : public rj::net::buffered_socket {
    public:
        typedef nlohmann::json Packet;

        Connection(rj::net::SOCKET sock, const sockaddr_storage &addr, const std::shared_ptr<ConnectionState> &state );

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

    private:
        std::shared_ptr<ConnectionState> state_;
    };

}

#endif //YAHTSEE_CONNECTION_H
