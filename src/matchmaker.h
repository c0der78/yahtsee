#ifndef _MATCHMAKER_H_
#define _MATCHMAKER_H_

#include <arg3net/socket_server.h>
#include <arg3net/http_client.h>
#include <string>
#include "client.h"
#include <arg3dice/yaht/player.h>

class game;
class player;

class matchmaker
{
public:

    matchmaker(game *game);
    matchmaker(const matchmaker &) = delete;
    matchmaker(matchmaker  &&other);
    matchmaker &operator=(const matchmaker &) = delete;
    matchmaker &operator=(matchmaker && );
    virtual ~matchmaker();
    void stop();
    bool is_valid() const;
    bool host(std::string *error = NULL, int port = INVALID);
    bool join_best_game(std::string *error = NULL);
    void notify_player_joined(const shared_ptr<player> &p);
    void notify_player_left(const shared_ptr<player> &p);
    void notify_game_start();
    void notify_player_roll();
    void notify_player_turn_finished();
private:

    void unregister();

    constexpr static const char *GAME_TYPE = "yahtsee";

    static const char *GAME_API_URL;

    static const int INVALID = -1;

    std::string gameId_;
    arg3::net::http_client api_;
    client client_;
    connection_factory client_factory_;
    arg3::net::socket_server server_;
    game *game_;

};

#endif
