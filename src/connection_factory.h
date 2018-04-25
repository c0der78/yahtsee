//
// Created by Ryan Jennings on 2018-02-27.
//

#ifndef YAHTSEE_CONNECTION_FACTORY_H
#define YAHTSEE_CONNECTION_FACTORY_H

#include <memory>
#include <coda/net/socket_factory.h>

namespace yahtsee {

    class StateManager;
    class Connection;

    class ConnectionFactory : public coda::net::socket_factory {
    public:
        ConnectionFactory(StateManager *state);

        std::shared_ptr<coda::net::buffered_socket> create_socket(const server_type &server, coda::net::SOCKET sock,
                                                                const sockaddr_storage &addr);

        // perform an operation on each connection
        void for_connections(std::function<void(const std::shared_ptr<Connection> &sock)> funk);

    private:
        std::vector<std::shared_ptr<Connection>> connections_;
        StateManager *state_;
    };


}

#endif //YAHTSEE_CONNECTION_FACTORY_H
