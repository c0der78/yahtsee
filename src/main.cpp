
#include <stdlib.h>
#include <iostream>
#include <arg3dice/yaht/engine.h>

#include "caca.h"

#include "yaht_game.h"

#include <arg3net/http_client.h>
#include <arg3/str_util.h>
#include <cassert>

using namespace std;

using namespace arg3;

static const char *GAME_TYPE = "yahtsee";

int main(int argc, char **argv)
{
    yaht_game game;

    srand(time(0));

    int port = (rand() % 99999) + 1024;

    char buf[BUFSIZ + 1] = {0};

    cout << "Registering game..." << endl;

    net::http_client api("connect.arg3.com");

    api.add_header("X-Application-Id", "51efcb5839a64a928a86ba8f2827b31d");

    api.add_header("X-Application-Token", "78ed4bfb42f54c9fa7ac62873d37228e");

    // net::http_client api("localhost:1337");

    // api.add_header("X-Application-Id", "8846b98d082d440b8d6024d723d7bc24");

    // api.add_header("X-Application-Token", "ac8afc408f284eedad323e1ddd5c17e4");

    snprintf(buf, BUFSIZ, "{\"type\": \"%s\", \"port\":%d}\n", GAME_TYPE, port);

    api.add_header("Content-Type", "application/json");

    api.set_payload(buf).post("api/v1/games/register");

    assert(api.response_code() == 200);

    string gameId = api.response();

    game.start();

    while (game.alive())
    {
        game.update();

        usleep(100);
    }

    game.reset();

    cout << "Unregistering game..." << endl;

    api.set_payload(gameId).post("api/v1/games/unregister");

    assert(api.response_code() == 200);

    return 0;
}

