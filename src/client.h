#ifndef YAHTSEE_CLIENT_H
#define YAHTSEE_CLIENT_H

#include <coda/net/buffered_socket.h>
#include <coda/net/uri.h>
#include <coda/net/socket_server.h>
#include <map>
#include <thread>
#include "connection.h"

namespace yahtsee {

    class Game;

    class Player;

    enum class ClientAction : int {
        INIT,
        START,
        PLAYER_JOINED,
        PLAYER_LEFT,
        PLAYER_ROLL,
        PLAYER_TURN_FINISHED
    };

    /**
     * A client is the instance of the connection connecting to the host.
     */
    class Client : public Connection {
    public:
        Client(coda::net::SOCKET sock, const sockaddr_storage &addr, const std::shared_ptr<ConnectionState> &state);

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
}

#endif
