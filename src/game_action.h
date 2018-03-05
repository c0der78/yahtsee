//
// Created by Ryan Jennings on 2018-03-04.
//

#ifndef YAHTSEE_GAMEACTION_H
#define YAHTSEE_GAMEACTION_H

#include <memory>
#include <rj/dice/yaht/scoresheet.h>
#include "player.h"

namespace yahtsee {

    class GameLogic;
    class Player;
    class Connection;

    class GameAction {
    public:

        GameAction(GameLogic *game);

        void add_network_player(const Player::Ref &p);

        void remove_network_player(Connection *c);

        void joined_game();

        void host_game();

        void join_game();

        void join_online_game();

        void network_player_joined(const Player::Ref &p);

        void network_player_left(const Player::Ref &p);

        void roll_dice();

        void select_die(const Player::Ref &player, int d);

        void lower_score(const Player::Ref &player, rj::yaht::scoresheet::type type);

        void score(const Player::Ref &player, int n);

        void score_best(const Player::Ref &player);

        void finish_turn();

        void disconnect();

        void network_player_finished(const Player::Ref &p);

        void game_over();
    private:
        GameLogic *game_;
    };

}

#endif //YAHTSEE_GAMEACTION_H
