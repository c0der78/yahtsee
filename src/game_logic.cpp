//
// Created by Ryan Jennings on 2018-03-04.
//

#include "game_logic.h"

namespace yahtsee
{
    bool GameLogic::is_thinking() const {
        return (flags_ & FINISHED) == 0;
    }

    void GameLogic::enlighten() {
        flags_ |= FINISHED;
    }


    Settings &GameLogic::settings() {
        return settings_;
    }

}