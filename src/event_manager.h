//
// Created by Ryan Jennings on 2018-03-04.
//

#ifndef YAHTSEE_GAMEACTION_H
#define YAHTSEE_GAMEACTION_H

#include <memory>
#include <coda/dice/yaht/scoresheet.h>
#include <fruit/fruit.h>

namespace yahtsee {

    class StateManager;
    class Player;
    class Connection;

    class EventManager {
    public:
        EventManager(StateManager *state);

        void joined_game();

        void host_game();

        void join_game();

        void join_online_game();

        void network_player_joined(const std::shared_ptr<Player> &p);

        void network_player_left(const std::shared_ptr<Player> &p);

        void roll_dice();

        void select_die(const std::shared_ptr<Player> &player, int d);

        void lower_score(const std::shared_ptr<Player> &player, coda::yaht::scoresheet::type type);

        void score(const std::shared_ptr<Player> &player, int n);

        void score_best(const std::shared_ptr<Player> &player);

        void finish_turn();

        void disconnect();

        void network_player_finished(const std::shared_ptr<Player> &p);

        void add_network_player(const std::shared_ptr<Player> &p);

        void remove_network_player(Connection *c);

        void game_over();
    private:
        StateManager *state_;
    };

}

#endif //YAHTSEE_GAMEACTION_H
