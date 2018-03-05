
#include "game.h"

int main(int argc, char **argv)
{
    int seed = 0;

    if (argc > 1) {
        seed = atoi(argv[1]);
    } else {
        seed = time(NULL);
    }

    yahtsee::Game game(seed);

    game.load();

    game.begin();

    while (game.on()) {
        game.update();
    }

    game.end();

    return 0;
}
