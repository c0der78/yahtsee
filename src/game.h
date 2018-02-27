//
// Created by Ryan Jennings on 2018-02-26.
//

#ifndef YAHTSEE_GAME_H
#define YAHTSEE_GAME_H

#include <memory>
#include "ui.h"

namespace yahtsee {
    class game {
    public:
        game(int seed);

        game &load();

        game &begin();

        bool on() const;

        game &update();

        game &end();

    private:
        std::unique_ptr<ui> ui_;
    };

}

#endif //YAHTSEE_GAME_H
