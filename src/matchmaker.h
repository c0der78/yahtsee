

#include <arg3net/socket_server.h>
#include <arg3net/http_client.h>
#include <string>

class yaht_game;

class matchmaker : public socket_server_listener
{
public:

    matchmaker();
    virtual ~matchmaker();
    void stop();
    int host(int port = INVALID);
private:

    constexpr static const char *GAME_TYPE = "yahtsee";

    constexpr static const char *GAME_API_URL = "connect.arg3.com";
    static const int INVALID = -1;

    shared_ptr<arg3::net::socket_server> server_;
    std::string gameId_;
    arg3::net::http_client api_;

};
