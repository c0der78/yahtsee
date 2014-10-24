
#include <stdlib.h>
#include <iostream>
#include <arg3dice/yaht/engine.h>

#include "caca.h"

#include "yaht_game.h"

#include <arg3net/http_client.h>
#include <arg3/util.h>

using namespace std;

using namespace arg3;

int main(int argc, char **argv)
{
    yaht_game game;

    net::http_client ipinfo("ipinfo.io");

    string response = ipinfo.get("ip").response();

    string host = arg3::trim(response);

    net::http_client api("localhost:1337");

    string payload = "{\"type\": \"yahtsee\", \"host\":\"" + host + "\", \"port\":6335}\n";

    api.add_header("Content-Type", "application/json");

    api.set_payload(payload).post("api/v1/games/register");

    game.start();

    while (game.alive())
    {
        game.update();

        //usleep(100);
    }

    return 0;
}

