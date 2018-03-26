//
// Created by Ryan Jennings on 2018-03-13.
//

#include "state_manager.h"
#include "game_state.h"

namespace yahtsee {

    StateManager::StateManager() : online_(this), events_(this), ui_(this) {
        state_ = std::make_shared<GameMenu>(this);
    }

    void StateManager::reset() {
        online_.reset();
        players_.reset();
    }

    void StateManager::render() {
        ui_.render();
    }

    void StateManager::update() {
        input_.update();
        ui_.update();
    }

    bool StateManager::is_finished() const {
        return !logic_.is_thinking();
    }

    GameLogic *StateManager::logic() { return &logic_; }
    GameUi *StateManager::ui() { return &ui_; }
    PlayerManager *StateManager::players() { return &players_; }
    InputManager *StateManager::input() { return &input_; }
    Multiplayer *StateManager::online() { return &online_; }
    EventManager *StateManager::events() { return &events_; }
}