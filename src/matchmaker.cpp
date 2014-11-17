#include "matchmaker.h"
#include <arg3json/json.h>

using namespace arg3;

#ifndef DEBUG
const char *matchmaker::GAME_API_URL = "connect.arg3.com";
#else
const char *matchmaker::GAME_API_URL = "localhost.arg3.com:1337";
#endif

matchmaker::matchmaker(yaht_game *game) : api_(GAME_API_URL), client_(game), client_factory_(game), server_(&client_factory_)
{
#ifndef DEBUG
    api_.add_header("X-Application-Id", "51efcb5839a64a928a86ba8f2827b31d");

    api_.add_header("X-Application-Token", "78ed4bfb42f54c9fa7ac62873d37228e");
#else
    api_.add_header("X-Application-Id", "8846b98d082d440b8d6024d723d7bc24");

    api_.add_header("X-Application-Token", "ac8afc408f284eedad323e1ddd5c17e4");
#endif
    api_.add_header("Content-Type", "application/json");
}

matchmaker::matchmaker(matchmaker &&other) :
    gameId_(std::move(other.gameId_)), api_(std::move(other.api_)), client_(std::move(other.client_)),
    client_factory_(std::move(other.client_factory_)), server_(std::move(other.server_))
{
    client_.set_non_blocking(true);
}


matchmaker &matchmaker::operator=(matchmaker && other)
{
    server_ = std::move(other.server_);
    client_ = std::move(other.client_);
    gameId_ = std::move(other.gameId_);
    api_ = std::move(other.api_);
    client_factory_ = std::move(other.client_factory_);

    return *this;
}

matchmaker::~matchmaker()
{
    stop();
}

void matchmaker::stop()
{
    server_.stop();

    client_.close();

    if (!gameId_.empty())
    {
        api_.set_payload(gameId_).post("api/v1/games/unregister");

        if (api_.response_code() != net::http::OK)
            cerr << "Unable to unregister game!" << endl;

        gameId_.clear();
    }
}

bool matchmaker::join_best_game(string *error)
{
    json::object type;

    type.set_string("type", GAME_TYPE);

    api_.set_payload(type.to_string()).post("api/v1/games/best");

    int response = api_.response_code();

    if (response != net::http::OK)
    {
        if (error)
        {
            *error = api_.response();
        }

        return false;
    }

    json::object game;

    game.parse(api_.response());

    string ip = game.get_string("host");

    int port = game.get_int("port");

    bool rval = client_.connect(ip, port);

    if (!rval)
    {
        char buf[BUFSIZ + 1] = {0};
        snprintf(buf, BUFSIZ, "Unable to connect to %s:%d", ip.c_str(), port);
        if (error)
            *error = buf;
    }

    return rval;
}

bool matchmaker::host(string *error, int port)
{
    if (port == INVALID)
    {
        port = (rand() % 99999) + 1024;
    }

    json::object json;

    json.set_string("type", GAME_TYPE);
    json.set_int("port", port);

    api_.set_payload(json.to_string()).post("api/v1/games/register");

    int response = api_.response_code();

    if (response != arg3::net::http::OK)
    {
        if (error)
            *error = api_.response();
        return false;
    }

    gameId_ = api_.response();

    server_.start_in_background(port);

    return true;
}

void matchmaker::notify_player_joined(const string &name)
{

}

void matchmaker::notify_player_left(const string &name)
{

}
