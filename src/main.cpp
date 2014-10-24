
#include <stdlib.h>
#include <iostream>
#include <arg3dice/yaht/engine.h>

#include "caca.h"

#include "yaht_game.h"

#include <arg3net/http_client.h>
#include <arg3/util.h>
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

    net::http_client api("arg3connect.herokuapp.com");
    //net::http_client api("localhost:1337");

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

