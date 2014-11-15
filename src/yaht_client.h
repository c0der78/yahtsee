#ifndef _YAHT_CLIENT_H_
#define _YAHT_CLIENT_H_

#include <arg3net/socket_factory.h>
#include <arg3net/buffered_socket.h>
#include <map>

class yaht_game;

typedef enum
{
    CLIENT_INIT
} client_action;

class yaht_client : public arg3::net::buffered_socket
{
public:
    yaht_client(yaht_game *game, arg3::net::SOCKET sock, const sockaddr_in &addr);
    yaht_client(yaht_game *game);

    virtual void on_will_read();
    virtual void on_did_read();
    virtual void on_will_write();
    virtual void on_did_write();
    virtual void on_connect();
    virtual void on_close();

private:
    yaht_game *game_;
};

class yaht_client_factory : public arg3::net::socket_factory
{
public:
    yaht_client_factory(yaht_game *game);

    std::shared_ptr<arg3::net::buffered_socket> create_socket(arg3::net::socket_server *server, arg3::net::SOCKET sock, const sockaddr_in &addr);

private:
    yaht_game *game_;
};

#endif
