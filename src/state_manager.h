//
// Created by Ryan Jennings on 2018-03-13.
//

#ifndef YAHTSEE_STATE_MANAGER_H
#define YAHTSEE_STATE_MANAGER_H

#include "game_manager.h"
#include "game_logic.h"
#include "game_ui.h"
#include "player_manager.h"
#include "input_manager.h"
#include "multiplayer.h"
#include "event_manager.h"

namespace yahtsee {

    class GameState;

    class StateManager : public GameManager {
    public:
        StateManager();
        void update();
        void render();
        bool is_finished() const;
        void reset();

        GameLogic *logic();
        GameUi *ui();
        PlayerManager *players();
        InputManager *input();
        Multiplayer *online();
        EventManager *events();
    private:
        GameLogic logic_;
        CursesUi ui_;
        PlayerManager players_;
        InputManager input_;
        Multiplayer online_;
        EventManager events_;
        std::shared_ptr<GameState> state_;
    };

}

#endif //YAHTSEE_STATE_MANAGER_H
