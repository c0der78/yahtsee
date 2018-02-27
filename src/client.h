#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <rj/net/buffered_socket.h>
#include <rj/net/socket_factory.h>
#include <nlohmann/json.hpp>
#include <rj/net/uri.h>
#include <rj/net/socket_server.h>
#include <map>
#include <thread>
#include "connection.h"

namespace yahtsee {

    class Game;

    class Player;

    typedef enum {
        CONNECTION_INIT,
        GAME_START,
        PLAYER_JOINED,
        PLAYER_LEFT,
        PLAYER_ROLL,
        PLAYER_TURN_FINISHED
    } ClientAction;


/*!
 * A client is the instance of the connection connecting to the host.
 */
    class Client : public Connection {
    public:
        Client(rj::net::SOCKET sock, const sockaddr_storage &addr);

        Client();

        Client(const Client &other) = delete;

        Client(Client &&other);

        virtual ~Client();

        Client &operator=(const Client &other) = delete;

        Client &operator=(Client &&other);

        virtual void on_connect(const std::shared_ptr<Player> &);

        virtual void on_close();

        /*! starts this client's io loop in the background */
        bool start_in_background(const std::string &host, int port);

        /*! starts this client's io loop */
        bool start(const std::string &host, int port);

    private:
        void on_will_read();

        void on_did_read();

        void on_will_write();

        void on_did_write();

        void run();

        std::shared_ptr<std::thread> backgroundThread_;
    };

    class ConnectionFactory : public rj::net::socket_factory {
    public:
        ConnectionFactory() = default;

        std::shared_ptr<rj::net::buffered_socket> create_socket(const server_type &server, rj::net::SOCKET sock,
                                                                const sockaddr_storage &addr);

        // perform an operation on each connection
        void for_connections(std::function<void(const std::shared_ptr<Connection> &sock)> funk);

    private:
        std::vector<std::shared_ptr<Connection>> connections_;
    };

}

#endif
