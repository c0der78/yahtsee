
#include <stdlib.h>
#include <iostream>
#include <arg3dice/yaht/engine.h>

#include "caca.h"

#include "yaht_game.h"

using namespace std;


int main(int argc, char **argv)
{
    yaht_game game;

    game.start();

    while (game.alive())
    {
        game.update();

        //usleep(100);
    }

    return 0;
}

