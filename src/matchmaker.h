#ifndef _MATCHMAKER_H_
#define _MATCHMAKER_H_

#include <arg3net/socket_server.h>
#include <arg3net/http_client.h>
#include <string>
#include "yaht_client.h"

class yaht_game;

class matchmaker
{
public:

    matchmaker(yaht_game *game);
    virtual ~matchmaker();
    void stop();
    bool host(int port = INVALID);
    bool join_best_game();
    void notify_player_joined(const string &name);
    void notify_player_left(const string &name);
private:

    constexpr static const char *GAME_TYPE = "yahtsee";

    constexpr static const char *GAME_API_URL = "connect.arg3.com";

    static const int INVALID = -1;

    shared_ptr<arg3::net::socket_server> server_;
    shared_ptr<yaht_client> client_;
    yaht_game *game_;
    std::string gameId_;
    arg3::net::http_client api_;
    yaht_client_factory client_factory_;

};

#endif
