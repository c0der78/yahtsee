//
// Created by Ryan Jennings on 2018-02-26.
//

#ifndef YAHTSEE_GAME_H
#define YAHTSEE_GAME_H

#include <memory>

#include "state_manager.h"

namespace yahtsee {

    class Game {
    public:
        Game(int seed);

        Game &load();

        Game &begin();

        bool on() const;

        Game &update();

        Game &render();

        Game &end();

    private:
        StateManager state_;
    };

}

#endif //YAHTSEE_GAME_H
