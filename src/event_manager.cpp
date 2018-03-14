//
// Created by Ryan Jennings on 2018-03-04.
//

#include "event_manager.h"
#include "game.h"
#include "player.h"
#include "state_manager.h"
#include <rj/log/log.h>

using namespace rj;

namespace yahtsee {

    EventManager::EventManager(StateManager *state) : state_(state) {}

    //! start hosting a game
    void EventManager::host_game()
    {
        string error;

        // the default (random) port
        auto port = -1;

        // display the hosting game modal
        state_->ui()->hosting_game();

        auto settings = state_->logic()->settings();

        // check if the settings specify a port
        if (settings.count("port")) {
            port = settings["port"];
        }

        bool response;

        try {
            // tell the matchmaker to host on a port
            response = state_->online()->host(settings, false, false, &error, port);
        } catch (const std::exception &e) {
            error = e.what();
            response = false;
        }

        // check for error
        if (!response) {
            log::trace("could not host game ", error.c_str());

            state_->ui()->flash_alert({"Unable to register game at this time.", error});

            state_->reset();

            return;
        }

        // inform the user we're hosting
        log::trace("waiting for connections");

        // TODO: Change state to waiting for connections
    }

    //! join a game posted in online registry
    void EventManager::join_online_game()
    {
        state_->ui()->modal_alert("Finding game to join...");

        string error;

        bool result;

        try {
            // tell the matchmaker to join an available game
            result = state_->online()->join_best_game(&error);
        } catch (const std::exception &e) {
            result = false;
        }

        // check for error
        if (!result) {
            log::trace("could not join game ", error.c_str());

            state_->ui()->flash_alert({"Unable to find game to join at this time.", error});

            state_->reset();
        }
    }

    //! join a specific game
    void EventManager::join_game()
    {
        state_->ui()->modal_alert("Attempting to join game...");

        string error;

        bool result;

        auto settings = state_->logic()->settings();

        // grab the host/port from the settings
        // this are piggy backed into the settings from user input
        string host = settings["lan_host"];

        int port = settings["lan_port"];

        try {
            // tell the matchmaker to join the game
            result = state_->online()->join_game(host, port, &error);
        } catch (const std::exception &e) {
            result = false;
        }

        // check for error
        if (!result) {
            log::trace("could not join game ", error.c_str());

            state_->ui()->flash_alert({"Unable to join game!", error});

            state_->reset();
        }
    }

    //! the user has joined a game
    void EventManager::joined_game()
    {
        // TODO: set state to waiting to start
    }

    //! the user has been disconnected
    void EventManager::disconnect()
    {
        if (state_->online()->is_matchmaking()) {
            state_->online()->notify_player_left(state_->players()->self());
        }
    }

    //! a player has been added to the game
    void EventManager::add_network_player(const std::shared_ptr<Player> &player)
    {
        char buf[BUFSIZ + 1] = {0};

        if (!state_->online()->is_matchmaking() || state_->players()->find_by_id(player->id())) {
            return;
        }

        log::trace("adding network player ", player->name().c_str(), " (", player->id().c_str(), ")");

        // add the player to the list
        state_->players()->add(player);

        // inform the matchmaker a player joined
        state_->online()->notify_player_joined(player);

        state_->ui()->waiting_for_players();
    }

    //! remove a player from the game
    void EventManager::remove_network_player(Connection *c) {
        // find the player to remove based on the connection
        auto player = state_->players()->find([&c](const std::shared_ptr<Player> &p) { return p->connection() == c; });

        state_->players()->remove(player);

        if (state_->players()->is_single_player()) {
            state_->reset();
        }

        state_->ui()->flash_alert(player->name() + " has left the game.");

        // inform the matchmaker a player left
        state_->online()->notify_player_left(player);
    }

    //! a network player has joined the hosted game
    void EventManager::network_player_joined(const std::shared_ptr<Player> &p)
    {
        log::trace("action network player joined");

        // add player to the list
        state_->players()->add(p);

        state_->ui()->client_waiting_to_start();
    }

    void EventManager::network_player_left(const std::shared_ptr<Player> &p)
    {
        state_->players()->remove(p);

        if (state_->players()->is_single_player()) {
            log::trace("reseting game");

            state_->reset();
        }

        state_->ui()->flash_alert(p->name() + " has left the game.");
    }

    void EventManager::roll_dice()
    {
        auto player = state_->players()->turn();

        if (!state_->players()->is_single_player() && player->id() != state_->players()->self()->id()) {
            state_->ui()->flash_alert("Its not your turn.");
            return;
        }

        if (player->roll_count() < 3) {
            player->roll();

            state_->online()->notify_player_roll(player);
        } else {
            state_->ui()->flash_alert({"You must choose a score after three rolls.", "Press '?' for help on how to score."});
        }
    }

    void EventManager::finish_turn()
    {
        state_->online()->notify_player_turn_finished(state_->players()->turn());

        state_->players()->next_turn();

        state_->ui()->flash_alert("It is now " + state_->players()->turn()->name() + "'s turn.");
    }

    void EventManager::network_player_finished(const std::shared_ptr<Player> &p)
    {
        state_->players()->next_turn();

        if (state_->players()->turn()->id() == state_->players()->self()->id()) {
            state_->ui()->flash_alert("It is now your turn.");
        } else {
            state_->ui()->set_needs_refresh();
        }
    }

    void EventManager::select_die(const std::shared_ptr<Player> &player, int d)
    {
        if (player->is_kept(d)) {
            player->keep_die(d, false);
        } else {
            player->keep_die(d, true);
        }
    }

    void EventManager::lower_score(const std::shared_ptr<Player> &player, yaht::scoresheet::type type)
    {
        if (!player->score().lower_played(type)) {
            player->score().lower_score(type, player->calculate_lower_score(type));

            finish_turn();
        } else {
            state_->ui()->already_scored();
        }
    }

    void EventManager::score(const std::shared_ptr<Player> &player, int n)
    {
        if (!player->score().upper_played(n)) {
            player->score().upper_score(n, player->calculate_upper_score(n));

            finish_turn();
        } else {
            state_->ui()->already_scored();
        }
    }

    void EventManager::score_best(const std::shared_ptr<Player> &player)
    {
        auto best_upper = player->calculate_best_upper_score();

        auto best_lower = player->calculate_best_lower_score();

        if (best_upper.second > best_lower.second) {
            if (!player->score().upper_played(best_upper.first)) {
                player->score().upper_score(best_upper.first, best_upper.second);

                finish_turn();
            } else {
                state_->ui()->already_scored();
            }

        } else if (best_lower.second > 0) {
            if (!player->score().lower_played(best_lower.first)) {
                player->score().lower_score(best_lower.first, best_lower.second);

                finish_turn();
            } else {
                state_->ui()->already_scored();
            }
        } else if (!player->score().lower_played(best_lower.first)) {
            player->score().lower_score(best_lower.first, best_lower.second);

            finish_turn();
        } else if (!player->score().upper_played(best_upper.first)) {
            player->score().upper_score(best_upper.first, best_upper.second);

            finish_turn();
        } else {
            state_->ui()->flash_alert("No best score found!");
        }
    }

    void EventManager::game_over()
    {
        if (state_->players()->is_single_player()) {
            state_->ui()->flash_alert("Game over.");
            return;
        }
        std::shared_ptr<Player> winner = nullptr;

        bool tieGame = false;

        for (const auto &p : state_->players()->all()) {

            if (!winner || p->score().total_score() > winner->score().total_score()) {
                winner = p;
            }
        }

        for (const auto &p : state_->players()->all()) {

            if (p != winner && p->score().total_score() == winner->score().total_score()) {
                tieGame = true;
                break;
            }
        }

        if (tieGame) {
            state_->ui()->flash_alert("Tie game!");
        } else if (winner == state_->players()->self()) {
            state_->ui()->flash_alert("You win!");
        } else {
            char buf[BUFSIZ + 1] = {0};
            snprintf(buf, BUFSIZ, "%s wins with %u points!", winner->name().c_str(), winner->score().total_score());
            state_->ui()->flash_alert(buf);
        }
    }


}