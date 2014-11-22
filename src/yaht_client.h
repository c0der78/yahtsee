#ifndef _YAHT_CLIENT_H_
#define _YAHT_CLIENT_H_

#include <arg3net/socket_factory.h>
#include <arg3net/buffered_socket.h>
#include <map>
#include <thread>

class yaht_game;

typedef enum
{
    CONNECTION_INIT, CLIENT_INIT, GAME_START
} client_action;

/*!
 * A connection is a remote user connected to this host instance
 */
class yaht_connection : public arg3::net::buffered_socket
{
public:
    yaht_connection(yaht_game *game, arg3::net::SOCKET sock, const sockaddr_storage &addr);
    yaht_connection(yaht_game *game);
    yaht_connection(const yaht_connection &other) = delete;
    yaht_connection(yaht_connection &&other);
    virtual ~yaht_connection();

    yaht_connection &operator=(const yaht_connection &other) = delete;
    yaht_connection &operator=(yaht_connection && other);

    virtual void on_will_read();
    virtual void on_did_read();
    virtual void on_will_write();
    virtual void on_did_write();
    virtual void on_connect();
    virtual void on_close();

protected:

    yaht_game *game_;
};

/*!
 * A client is the instance of the connection connecting to the host.
 */
class yaht_client : public yaht_connection
{
public:
    yaht_client(yaht_game *game, arg3::net::SOCKET sock, const sockaddr_storage &addr);
    yaht_client(yaht_game *game);
    yaht_client(const yaht_client &other) = delete;
    yaht_client(yaht_client &&other);
    virtual ~yaht_client();

    yaht_client &operator=(const yaht_client &other) = delete;
    yaht_client &operator=(yaht_client && other);

    virtual void on_connect();

    bool start_in_background(const std::string &host, int port);
    bool start(const std::string &host, int port);

private:

    void run();

    shared_ptr<thread> backgroundThread_;
};

class yaht_connection_factory : public arg3::net::socket_factory
{
public:
    yaht_connection_factory(yaht_game *game);

    std::shared_ptr<arg3::net::buffered_socket> create_socket(arg3::net::socket_server *server, arg3::net::SOCKET sock, const sockaddr_storage &addr);

    void for_connections(std::function<void(const shared_ptr<yaht_connection> &sock)> funk);
private:
    yaht_game *game_;
    vector<shared_ptr<yaht_connection>> connections_;
};

#endif
