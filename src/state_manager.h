//
// Created by Ryan Jennings on 2018-03-13.
//

#ifndef YAHTSEE_STATE_MANAGER_H
#define YAHTSEE_STATE_MANAGER_H

#include "game_logic.h"
#include "game_ui.h"
#include "player_manager.h"
#include "input_manager.h"
#include "multiplayer.h"
#include "event_manager.h"

namespace yahtsee {

    class GameState;

    class StateManager : public Updatable, public Renderable {
    public:
        StateManager();

        void update();
        void render();
        bool is_finished() const;
        void reset();

        GameLogic *logic();
        std::shared_ptr<GameUi> ui();
        PlayerManager *players();
        InputManager *input();
        Multiplayer *online();
        EventManager *events();

        void set(const std::shared_ptr<GameState> &value);

    private:

        GameLogic logic_;
        std::shared_ptr<GameUi> ui_;
        PlayerManager players_;
        InputManager input_;
        Multiplayer online_;
        EventManager events_;
        std::shared_ptr<GameState> state_;
    };

}

#endif //YAHTSEE_STATE_MANAGER_H
