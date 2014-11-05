#include "matchmaker.h"
#include <arg3json/json.h>

using namespace arg3;

matchmaker::matchmaker(yaht_game *game) : server_(nullptr), client_(nullptr), game_(game), api_(GAME_API_URL), client_factory_(game)
{
    api_.add_header("X-Application-Id", "51efcb5839a64a928a86ba8f2827b31d");

    api_.add_header("X-Application-Token", "78ed4bfb42f54c9fa7ac62873d37228e");

    api_.add_header("Content-Type", "application/json");
}

matchmaker::~matchmaker()
{
    stop();
}

void matchmaker::stop()
{
    if (!gameId_.empty())
    {
        api_.set_payload(gameId_).post("api/v1/games/unregister");

        if (api_.response_code() != net::http::OK)
            cerr << "Unable to unregister game!" << endl;

        gameId_.clear();
    }

    if (server_ != nullptr)
    {
        server_->stop();
        server_ = nullptr;
    }

    if (client_ != nullptr)
    {
        client_->close();
        client_ = nullptr;
    }
}

bool matchmaker::join_best_game()
{
    api_.post("api/v1/games/best");

    int response = api_.response_code();

    if (response != net::http::OK)
        return false;

    json::object game = json::object(api_.response());

    string ip = game.get_string("ip");

    int port = game.get_int("port");

    client_ = make_shared<yaht_client>(game_);

    return client_->connect(ip, port);
}

bool matchmaker::host(int port)
{
    char buf[BUFSIZ + 1] = {0};

    if (port == INVALID)
    {
        port = (rand() % 99999) + 1024;
    }

    snprintf(buf, BUFSIZ, "{\"type\": \"%s\", \"port\":%d}\n", GAME_TYPE, port);

    api_.set_payload(buf).post("api/v1/games/register");

    int response = api_.response_code();

    if (response != arg3::net::http::OK)
        return false;

    gameId_ = api_.response();

    server_ = make_shared<arg3::net::socket_server>(port);

    server_->start_in_background();

    return true;
}

void matchmaker::notify_player_joined(const string &name)
{

}

void matchmaker::notify_player_left(const string &name)
{

}