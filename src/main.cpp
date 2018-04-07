
#include "game.h"
#include "game_ui.h"
#include <thread>

int main(int argc, char **argv)
{
    int seed = 0;

    if (argc > 1) {
        // custom seeding for better functional testing
        seed = atoi(argv[1]);
    } else {
        seed = time(NULL);
    }

    yahtsee::Game game(seed);

    game.load();

    game.begin();

    while (game.on()) {

        game.render();

        game.update();

        std::this_thread::yield();
    }

    game.end();

    return 0;
}
