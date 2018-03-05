//
// Created by Ryan Jennings on 2018-02-27.
//

#ifndef YAHTSEE_CONNECTION_FACTORY_H
#define YAHTSEE_CONNECTION_FACTORY_H

#include <memory>
#include <rj/net/socket_factory.h>

namespace yahtsee {

    class GameLogic;
    class Connection;

    class ConnectionFactory : public rj::net::socket_factory {
    public:
        ConnectionFactory(GameLogic *game);

        std::shared_ptr<rj::net::buffered_socket> create_socket(const server_type &server, rj::net::SOCKET sock,
                                                                const sockaddr_storage &addr);

        // perform an operation on each connection
        void for_connections(std::function<void(const std::shared_ptr<Connection> &sock)> funk);

    private:
        std::vector<std::shared_ptr<Connection>> connections_;
        GameLogic *game_;
    };


}

#endif //YAHTSEE_CONNECTION_FACTORY_H
