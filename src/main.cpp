
#include <stdlib.h>

#include <caca.h>

#include "game.h"

using namespace std;

using namespace rj;

int main(int argc, char **argv)
{
    ::game game;

    srand(time(0));

    game.load_settings(argv[0]);

    game.start();

    while (game.alive()) {
        game.update();
    }

    game.reset();

    return 0;
}
