#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <rj/net/buffered_socket.h>
#include <rj/net/socket_factory.h>
#include <json.hpp>
#include <map>
#include <thread>

class game;

typedef enum {
    // this (host) connection initializing
    CONNECTION_INIT,
    // remote (client) connection initializing
    REMOTE_CONNECTION_INIT,
    GAME_START,
    PLAYER_JOINED,
    PLAYER_LEFT,
    PLAYER_ROLL,
    PLAYER_TURN_FINISHED
} client_action;


/*!
 * A connection is a remote user connected to this host instance
 */
class connection : public rj::net::buffered_socket
{
   public:
    typedef nlohmann::json packet_format;
    connection(game *game, rj::net::SOCKET sock, const sockaddr_storage &addr);
    connection(game *game);
    /* non copyable */
    connection(const connection &other) = delete;
    connection(connection &&other);
    virtual ~connection();

    /* non copyable */
    connection &operator=(const connection &other) = delete;
    connection &operator=(connection &&other);

    virtual void on_will_read();
    virtual void on_did_read();
    virtual void on_will_write();
    virtual void on_did_write();
    virtual void on_connect();
    virtual void on_close();

   protected:
    void handle_player_roll(const packet_format &);
    void handle_game_start(const packet_format &);
    void handle_connection_init(const packet_format &);
    void handle_remote_connection_init(const packet_format &);
    void handle_player_joined(const packet_format &);
    void handle_player_left(const packet_format &);
    void handle_player_turn_finished(const packet_format &);
    game *game_;
};

/*!
 * A client is the instance of the connection connecting to the host.
 */
class client : public connection
{
   public:
    client(game *game, rj::net::SOCKET sock, const sockaddr_storage &addr);
    client(game *game);
    client(const client &other) = delete;
    client(client &&other);
    virtual ~client();

    client &operator=(const client &other) = delete;
    client &operator=(client &&other);

    virtual void on_connect();
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

class connection_factory : public rj::net::socket_factory
{
   public:
    connection_factory(game *game);

    std::shared_ptr<rj::net::buffered_socket> create_socket(const server_type &server, rj::net::SOCKET sock,
                                                            const sockaddr_storage &addr);

    // perform an operation on each connection
    void for_connections(std::function<void(const std::shared_ptr<connection> &sock)> funk);

   private:
    game *game_;
    std::vector<std::shared_ptr<connection>> connections_;
};

#endif
