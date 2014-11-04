
#include <stdlib.h>
#include <iostream>
#include <arg3dice/yaht/engine.h>

#include "caca.h"

#include "yaht_game.h"

#include <cassert>

using namespace std;

using namespace arg3;

int main(int argc, char **argv)
{
    yaht_game game;

    srand(time(0));

    game.start();

    while (game.alive())
    {
        game.update();

        usleep(100);
    }

    game.reset();

    return 0;
}

