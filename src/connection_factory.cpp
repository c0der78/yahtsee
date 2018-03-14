//
// Created by Ryan Jennings on 2018-02-27.
//

#include "connection_factory.h"
#include "connection.h"
#include "connection_state.h"
#include "game_logic.h"
#include "game_ui.h"


namespace yahtsee {

    typedef std::function <void(const std::shared_ptr <Connection> &conn)> ConnectionPtr;

    ConnectionFactory::ConnectionFactory(StateManager *state) : state_(state) {}

    std::shared_ptr<rj::net::buffered_socket> ConnectionFactory::create_socket(const server_type &server,
                                                                                rj::net::SOCKET sock,
                                                                       const sockaddr_storage &addr) {

        auto state = std::make_shared<ConnectionState>(state_);

        auto socket = std::make_shared<Connection>(sock, addr, state);

        // add to the list of connections
        connections_.push_back(socket);

        return socket;
    }

    void ConnectionFactory::for_connections(ConnectionPtr funk) {
        for (const auto &c : connections_) {
            funk(c);
        }
    }
}
