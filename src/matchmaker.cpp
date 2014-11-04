#include "matchmaker.h"

matchmaker::matchmaker() : server_(nullptr), api_(GAME_API_URL)
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

        if (api_.response_code() != arg3::net::http::OK)
            cerr << "Unable to unregister game!" << endl;

        gameId_.clear();
    }

    if (server_ != nullptr)
    {
        server_->stop();
        server_ = nullptr;
    }
}

int matchmaker::host(int port)
{
    char buf[BUFSIZ + 1] = {0};

    if (port == INVALID)
    {
        port = (rand() % 99999) + 1024;
    }

    snprintf(buf, BUFSIZ, "{\"type\": \"%s\", \"port\":%d}\n", GAME_TYPE, port);

    api_.set_payload(buf).post("api/v1/games/register");

    int response = api_.response_code();

    if (response == arg3::net::http::OK)
    {
        gameId_ = api_.response();

        server_ = make_shared<arg3::net::socket_server>(port);

        server_->start_in_background();
    }
    return response;
}
