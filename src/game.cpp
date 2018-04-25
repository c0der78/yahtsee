//
// Created by Ryan Jennings on 2018-02-26.
//

#include "game.h"

namespace yahtsee {

    Game::Game(int seed) : state_() {

    }

    Game &Game::load() {

        return *this;
    }

    Game &Game::begin() {

        return *this;
    }

    bool Game::on() const {
        return !state_.is_finished();
    }

    Game &Game::update() {

        state_.update();

        return *this;
    }

    Game &Game::render() {

        state_.render();

        return *this;
    }

    Game &Game::end() {

        return *this;
    }
}

