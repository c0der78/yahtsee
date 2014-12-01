#ifndef _client_H_
#define _client_H_

#include <arg3net/socket_factory.h>
#include <arg3net/buffered_socket.h>
#include <map>
#include <thread>

class game;

typedef enum
{
    CONNECTION_INIT, CLIENT_INIT, GAME_START, PLAYER_JOINED, PLAYER_LEFT
} client_action;

/*!
 * A connection is a remote user connected to this host instance
 */
class connection : public arg3::net::buffered_socket
{
public:
    connection(game *game, arg3::net::SOCKET sock, const sockaddr_storage &addr);
    connection(game *game);
    connection(const connection &other) = delete;
    connection(connection &&other);
    virtual ~connection();

    connection &operator=(const connection &other) = delete;
    connection &operator=(connection && other);

    virtual void on_will_read();
    virtual void on_did_read();
    virtual void on_will_write();
    virtual void on_did_write();
    virtual void on_connect();
    virtual void on_close();

protected:

    game *game_;
};

/*!
 * A client is the instance of the connection connecting to the host.
 */
class client : public connection
{
public:
    client(game *game, arg3::net::SOCKET sock, const sockaddr_storage &addr);
    client(game *game);
    client(const client &other) = delete;
    client(client &&other);
    virtual ~client();

    client &operator=(const client &other) = delete;
    client &operator=(client && other);

    virtual void on_connect();
    virtual void on_close();

    bool start_in_background(const std::string &host, int port);
    bool start(const std::string &host, int port);

private:

    void run();

    shared_ptr<thread> backgroundThread_;
};

class connection_factory : public arg3::net::socket_factory
{
public:
    connection_factory(game *game);

    std::shared_ptr<arg3::net::buffered_socket> create_socket(arg3::net::socket_server *server, arg3::net::SOCKET sock, const sockaddr_storage &addr);

    void for_connections(std::function<void(const shared_ptr<connection> &sock)> funk);
private:
    game *game_;
    vector<shared_ptr<connection>> connections_;
};

#endif
