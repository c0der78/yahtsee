//
// Created by Ryan Jennings on 2018-03-06.
//

#include "game_state.h"
#include "game_ui.h"
#include "state_manager.h"

namespace yahtsee {

    WelcomeState::WelcomeState(StateManager *state) : GameState(state) {

        menu_->add_option("n", "Single Player Game", [&](const Menu &menu, const std::string &opt) {
                    state_->set(std::make_shared<NewSinglePlayerState>(state_));
                })
                .add_option("m", "Multiplayer Game")
                .add_option("s", "Settings")
                .add_option("x", "Exit", [&](const Menu &menu, const std::string &opt) {
                    state_->logic()->stop_thinking();
                });
    }

    void WelcomeState::render() {
        menu_->render();
    }

    void WelcomeState::update() {
        menu_->update();
    }

    NewSinglePlayerState::NewSinglePlayerState(StateManager *state) : GameState(state) {

    }

    void NewSinglePlayerState::render() {

    }

    void NewSinglePlayerState::update() {

    }
}