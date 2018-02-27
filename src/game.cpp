//
// Created by Ryan Jennings on 2018-02-26.
//

#include "game.h"


namespace yahtsee {


    game::game(int seed) {

    }

    game &game::load() {
        return *this;
    }

    game &game::begin() {
        return *this;
    }

    bool game::on() const {
        return false;
    }

    game &game::update() {
        return *this;
    }

    game &game::end() {
        return *this;
    }
}