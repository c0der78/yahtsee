//
// Created by Ryan Jennings on 2018-03-04.
//

#include "game_action.h"
#include "game.h"
#include <rj/log/log.h>

using namespace rj;

namespace yahtsee {



    //! start hosting a game
    void GameAction::host_game()
    {
        string error;

        // the default (random) port
        auto port = -1;

        // display the hosting game modal
        game_->ui()->hosting_game();

        auto settings = game_->settings();

        // check if the settings specify a port
        if (settings.count("port")) {
            port = settings["port"];
        }

        bool response;

        try {
            // tell the matchmaker to host on a port
            response = game_->online()->host(settings, false, false, &error, port);
        } catch (const std::exception &e) {
            error = e.what();
            response = false;
        }

        // check for error
        if (!response) {
            log::trace("could not host game ", error.c_str());

            game_->ui()->flash_alert({"Unable to register game at this time.", error});

            game_->reset();

            return;
        }

        // inform the user we're hosting
        log::trace("waiting for connections");

        // TODO: Change state to waiting for connections
    }

    //! join a game posted in online registry
    void GameAction::join_online_game()
    {
        game_->ui()->modal_alert("Finding game to join...");

        string error;

        bool result;

        try {
            // tell the matchmaker to join an available game
            result = game_->online()->join_best_game(&error);
        } catch (const std::exception &e) {
            result = false;
        }

        // check for error
        if (!result) {
            log::trace("could not join game ", error.c_str());

            game_->ui()->flash_alert({"Unable to find game to join at this time.", error});

            game_->reset();
        }
    }

    //! join a specific game
    void GameAction::join_game()
    {
        game_->ui()->modal_alert("Attempting to join game...");

        string error;

        bool result;

        auto settings = game_->settings();

        // grab the host/port from the settings
        // this are piggy backed into the settings from user input
        string host = settings["lan_host"];

        int port = settings["lan_port"];

        try {
            // tell the matchmaker to join the game
            result = game_->online()->join_game(host, port, &error);
        } catch (const std::exception &e) {
            result = false;
        }

        // check for error
        if (!result) {
            log::trace("could not join game ", error.c_str());

            game_->ui()->flash_alert({"Unable to join game!", error});

            game_->reset();
        }
    }

    //! the user has joined a game
    void GameAction::joined_game()
    {
        // TODO: set state to waiting to start
    }

    //! the user has been disconnected
    void GameAction::disconnect()
    {
        if (game_->online()->is_matchmaking()) {
            game_->online()->notify_player_left(game_->player());
        }
    }

    //! a player has been added to the game
    void GameAction::add_network_player(const Player::Ref &player)
    {
        char buf[BUFSIZ + 1] = {0};

        if (!game_->online()->is_matchmaking() || game_->find_player_by_id(player->id())) {
            return;
        }

        log::trace("adding network player ", player->name().c_str(), " (", player->id().c_str(), ")");

        // add the player to the list
        game_->add_player(player);

        // inform the matchmaker a player joined
        game_->online()->notify_player_joined(player);

        game_->ui()->waiting_for_players();
    }

    //! remove a player from the game
    void GameAction::remove_network_player(Connection *c) {
        // find the player to remove based on the connection
        auto player = game_->find_player([&c](const Player::Ref &p) { return p->connection() == c; });

        game_->remove_player(player);

        if (game_->is_single_player()) {
            game_->reset();
        }

        game_->ui()->flash_alert(player->name() + " has left the game.");

        // inform the matchmaker a player left
        game_->online()->notify_player_left(player);
    }

    //! a network player has joined the hosted game
    void GameAction::network_player_joined(const Player::Ref &p)
    {
        log::trace("action network player joined");

        // add player to the list
        game_->add_player(p);

        game_->ui()->client_waiting_to_start();
    }

    void GameAction::network_player_left(const Player::Ref &p)
    {
        game_->remove_player(p);

        if (game_->is_single_player()) {
            log::trace("reseting game");

            game_->reset();
        }

        game_->ui()->flash_alert(p->name() + " has left the game.");
    }

    void GameAction::roll_dice()
    {
        auto player = game_->turn();

        if (!game_->is_single_player() && player->id() != game_->player()->id()) {
            game_->ui()->flash_alert("Its not your turn.");
            return;
        }

        if (player->roll_count() < 3) {
            player->roll();

            game_->online()->notify_player_roll(player);
        } else {
            game_->ui()->flash_alert({"You must choose a score after three rolls.", "Press '?' for help on how to score."});
        }
    }

    void GameAction::finish_turn()
    {
        game_->online()->notify_player_turn_finished(game_->turn());

        game_->next_turn();

        game_->ui()->flash_alert("It is now " + game_->turn()->name() + "'s turn.");
    }

    void GameAction::network_player_finished(const Player::Ref &p)
    {
        game_->next_turn();

        if (game_->turn()->id() == game_->player()->id()) {
            game_->ui()->flash_alert("It is now your turn.");
        } else {
            game_->ui()->refresh();
        }
    }

    void GameAction::select_die(const Player::Ref &player, int d)
    {
        if (player->is_kept(d)) {
            player->keep_die(d, false);
        } else {
            player->keep_die(d, true);
        }
    }

    void GameAction::lower_score(const Player::Ref &player, yaht::scoresheet::type type)
    {
        if (!player->score().lower_played(type)) {
            player->score().lower_score(type, player->calculate_lower_score(type));

            finish_turn();
        } else {
            game_->ui()->already_scored();
        }
    }

    void GameAction::score(const Player::Ref &player, int n)
    {
        if (!player->score().upper_played(n)) {
            player->score().upper_score(n, player->calculate_upper_score(n));

            finish_turn();
        } else {
            game_->ui()->already_scored();
        }
    }

    void GameAction::score_best(const Player::Ref &player)
    {
        auto best_upper = player->calculate_best_upper_score();

        auto best_lower = player->calculate_best_lower_score();

        if (best_upper.second > best_lower.second) {
            if (!player->score().upper_played(best_upper.first)) {
                player->score().upper_score(best_upper.first, best_upper.second);

                finish_turn();
            } else {
                game_->ui()->already_scored();
            }

        } else if (best_lower.second > 0) {
            if (!player->score().lower_played(best_lower.first)) {
                player->score().lower_score(best_lower.first, best_lower.second);

                finish_turn();
            } else {
                game_->ui()->already_scored();
            }
        } else if (!player->score().lower_played(best_lower.first)) {
            player->score().lower_score(best_lower.first, best_lower.second);

            finish_turn();
        } else if (!player->score().upper_played(best_upper.first)) {
            player->score().upper_score(best_upper.first, best_upper.second);

            finish_turn();
        } else {
            game_->ui()->flash_alert("No best score found!");
        }
    }

    void GameAction::game_over()
    {
        if (game_->is_single_player()) {
            game_->ui()->flash_alert("Game over.");
            return;
        }
        Player::Ref winner = nullptr;

        bool tieGame = false;

        for (const auto &p : game_->players()) {

            if (!winner || p->score().total_score() > winner->score().total_score()) {
                winner = p;
            }
        }

        for (const auto &p : game_->players()) {

            if (p != winner && p->score().total_score() == winner->score().total_score()) {
                tieGame = true;
                break;
            }
        }

        if (tieGame) {
            game_->ui()->flash_alert("Tie game!");
        } else if (winner == game_->player()) {
            game_->ui()->flash_alert("You win!");
        } else {
            char buf[BUFSIZ + 1] = {0};
            snprintf(buf, BUFSIZ, "%s wins with %u points!", winner->name().c_str(), winner->score().total_score());
            game_->ui()->flash_alert(buf);
        }
    }


}