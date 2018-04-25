//
// Created by Ryan Jennings on 2018-03-06.
//

#ifndef YAHTSEE_GAME_STATE_H
#define YAHTSEE_GAME_STATE_H

#include <memory>

#include "renderable.h"
#include "updatable.h"

namespace yahtsee {

    class Player;
    class Connection;
    class StateManager;
    class Menu;

    class GameState : public Updatable, public Renderable {
    public:
        GameState(StateManager *state) : state_(state) {}

    protected:
        StateManager *state_;
    };

    class WelcomeState : public GameState {
    public:
        WelcomeState(StateManager *state);

        void update();
        void render();

    private:
        std::shared_ptr<Menu> menu_;
    };

    class NewSinglePlayerState : public GameState {
    public:
        NewSinglePlayerState(StateManager *state);

        void update();
        void render();
    };

    class WaitingForPlayerState : public GameState {
    private:

        void add_network_player(const std::shared_ptr<Player> &p);

        void remove_network_player(Connection *c);
    };

}

#endif //YAHTSEE_GAME_STATE_H
