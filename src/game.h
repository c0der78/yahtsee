//
// Created by Ryan Jennings on 2018-02-26.
//

#ifndef YAHTSEE_GAME_H
#define YAHTSEE_GAME_H

#include <memory>

#include "game_logic.h"

namespace yahtsee {

    class Game {
    public:
        Game(int seed);

        Game &load();

        Game &begin();

        bool on() const;

        Game &update();

        Game &end();

        std::unique_ptr<GameLogic> &logic();

    private:
        std::unique_ptr<GameLogic> logic_;
    };


}

#endif //YAHTSEE_GAME_H
