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
    matchmaker(const matchmaker &) = delete;
    matchmaker(matchmaker  &&other);
    matchmaker &operator=(const matchmaker &) = delete;
    matchmaker &operator=(matchmaker && );
    virtual ~matchmaker();
    void stop();
    bool host(std::string *error = NULL, int port = INVALID);
    bool join_best_game(std::string *error = NULL);
    void notify_player_joined(const string &name);
    void notify_player_left(const string &name);
private:

    constexpr static const char *GAME_TYPE = "yahtsee";

    static const char *GAME_API_URL;

    static const int INVALID = -1;

    std::string gameId_;
    arg3::net::http_client api_;
    yaht_client client_;
    yaht_connection_factory client_factory_;
    arg3::net::socket_server server_;

};

#endif
