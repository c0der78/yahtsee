//
// Created by Ryan Jennings on 2018-03-04.
//

#ifndef YAHTSEE_GAME_LOGIC_H
#define YAHTSEE_GAME_LOGIC_H

#include <functional>
#include <string>
#include <vector>
#include <memory>

#include <nlohmann/json.hpp>

#include "multiplayer.h"
#include "event_manager.h"

namespace yahtsee {

    class Player;

    using Settings = nlohmann::json;

    class GameLogic {
    private:

    public:

        GameLogic();

        // properties
        bool is_thinking() const noexcept;

        void stop_thinking() noexcept;

        Settings &settings();

    private:
        int flags_;
        Settings settings_;

        static const int FINISHED = (1 << 0);
    };

}


#endif //YAHTSEE_GAME_LOGIC_H
