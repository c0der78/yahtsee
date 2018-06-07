//
// Created by Ryan Jennings on 2018-03-04.
//

#include "game_logic.h"

namespace yahtsee
{
    GameLogic::GameLogic() : flags_(0) {
    }

    bool GameLogic::is_thinking() const noexcept {
        return (flags_ & FINISHED) == 0;
    }

    void GameLogic::stop_thinking() noexcept {
        flags_ |= FINISHED;
    }


    Settings &GameLogic::settings() {
        return settings_;
    }

}