#include "game.h"
#include "player.h"
#include "log.h"
#include <future>

using namespace arg3;

void game::action_host_game()
{
    string error;

    display_alert("Starting server...");

    auto port = -1;

    if (settings_.contains("port"))
    {
        port = settings_.get_int("port");
    }

    bool response;

    try
    {
        response = matchmaker_.host(&error, port);
    }
    catch ( const std::exception &e)
    {
        response = false;
    }

    pop_alert(); // done registration

    if (!response)
    {
        logf("could not host game %s", error.c_str());

        display_alert(2000, { "Unable to register game at this time.", error }, nullptr, [&]()
        {
            set_state(&game::state_multiplayer_menu);

            display_multiplayer_menu();
        });

        players_.clear();

        flags_ = 0;

        return;
    }

    logf("waiting for connections");

    set_state(&game::state_waiting_for_connections);

}

void game::action_joined_game()
{
    //pop_alert();

    set_state(&game::state_client_waiting_to_start);
}

void game::action_disconnect()
{
    if (flags_ & FLAG_JOINING)
    {
        matchmaker_.notify_player_left(this_player());
    }
}

void game::action_add_network_player(const shared_ptr<player> &player)
{
    char buf[BUFSIZ + 1] = {0};

    players_.push_back(player);

    matchmaker_.notify_player_joined(player);

    pop_alert();

    vector<string> message;

    int count = 1;

    for (const auto &p : players_)
    {
        snprintf(buf, BUFSIZ, "%2d: %s", count++, p->name().c_str());
        message.push_back(buf);
    }

    message.push_back(" ");

    message.push_back("Waiting for more players, press 's' to start the game.");

    display_alert(message);
}

void game::action_remove_network_player(connection *c)
{
    auto it = find_if(players_.begin(), players_.end(), [&c](const shared_ptr<player> &p)
    {
        return p->c0nnection() == c;
    });

    if (it != players_.end())
    {
        auto p = *it;

        players_.erase(it);

        if (players_.size() == 1)
        {
            players_.clear();

            display_alert(2000, p->name() + " has left the game.", nullptr, [&]()
            {
                set_state(&game::state_game_menu);

                display_game_menu();
            });
        }
        else
        {
            display_alert(2000, p->name() + " has left the game.");
        }
        matchmaker_.notify_player_left(p);

    }
}

void game::action_network_player_joined(const shared_ptr<player> &p)
{
    players_.push_back(p);

    display_client_waiting_to_start();
}

void game::action_network_player_left(const shared_ptr<player> &p)
{
    players_.erase(remove_if(players_.begin(), players_.end(), [&p](const shared_ptr<player> &o)
    {
        return p->id() == o->id();
    }), players_.end());

    if (players_.size() == 1)
    {
        logf("reseting game");

        set_state(&game::state_game_menu);

        players_.clear();
    }
    else
    {
        logf("still have %zu players", players_.size());
    }

    display_alert(2000, p->name() + " has left the game.", nullptr, [&]()
    {
        if (players_.empty())
            display_game_menu();
    });
}

void game::action_roll_dice()
{
    auto player = current_player();

    if (is_online() && player->id() != this_player()->id())
    {
        display_alert(2000, "Its not your turn.");
        return;
    }

    if ((flags_ & FLAG_ROLLING))
    {
        set_state(&game::state_rolling_dice);

        flags_ &= ~FLAG_ROLLING;
    }
    else if (player->roll_count() < 3)
    {
        player->roll();

        set_state(&game::state_rolling_dice);

        matchmaker_.notify_player_roll();
    }
    else
    {
        display_alert(2000, vector<string>({ "You must choose a score after three rolls.", "Press '?' for help on how to score." }));
    }
}

void game::action_finish_turn()
{
    matchmaker_.notify_player_turn_finished();

    flags_ &= ~FLAG_ROLLING;

    next_player();

    pop_state();
}

void game::action_network_player_finished(const shared_ptr<player> &p)
{
    next_player();

    if (current_player()->id() == this_player()->id())
    {
        display_alert(2000, "It is now your turn.", nullptr, [&]()
        {

            set_state(&game::state_playing);

            set_needs_display();

            set_needs_clear();
        });
    }
    else
    {

        set_needs_display();

        set_needs_clear();
    }
}

void game::action_select_die(shared_ptr<yaht::player> player, int d)
{
    if (player->is_kept(d))
        player->keep_die(d, false);
    else
        player->keep_die(d, true);

    auto box = displayed_alert();

    box.display();
}

void game::action_lower_score(shared_ptr<yaht::player> player, yaht::scoresheet::type type)
{
    if (!player->score().lower_played(type))
    {
        player->score().lower_score(type, player->calculate_lower_score(type));

        action_finish_turn();
    }
    else
    {
        display_already_scored();
    }
}

void game::action_score(shared_ptr<yaht::player> player, int n)
{
    if (!player->score().upper_played(n))
    {
        player->score().upper_score(n, player->calculate_upper_score(n));

        action_finish_turn();
    }
    else
    {
        display_already_scored();
    }
}

void game::action_score_best(shared_ptr<yaht::player> player)
{
    auto best_upper = player->calculate_best_upper_score();

    auto best_lower = player->calculate_best_lower_score();

    if (best_upper.second > best_lower.second)
    {
        if (!player->score().upper_played(best_upper.first))
        {
            player->score().upper_score(best_upper.first, best_upper.second);

            action_finish_turn();
        }
        else
        {
            display_already_scored();
        }

    }
    else if (best_lower.second > 0)
    {
        if (!player->score().lower_played(best_lower.first))
        {
            player->score().lower_score(best_lower.first, best_lower.second);

            action_finish_turn();
        }
        else
        {
            display_already_scored();
        }
    }
    else if (!player->score().lower_played(best_lower.first))
    {
        player->score().lower_score(best_lower.first, best_lower.second);

        action_finish_turn();
    }
    else if (!player->score().upper_played(best_upper.first))
    {
        player->score().upper_score(best_upper.first, best_upper.second);

        action_finish_turn();
    }
    else
    {
        display_alert(2000, "No best score found!");
    }
}

void game::action_game_over()
{
    if (players_.size() == 1)
    {
        display_alert(4000, "Game over.", nullptr, [&]()
        {
            set_state(&game::state_game_menu);
        });
        return;
    }
    shared_ptr<player> winner = nullptr;

    bool tieGame = false;

    for (size_t i = 0; i < players_.size(); i++)
    {
        auto p = players_[i];

        if (!winner || p->score().total_score() > winner->score().total_score())
        {
            winner = p;
        }
    }

    for (size_t i = 0; i < players_.size(); i++)
    {
        auto p = players_[i];

        if (p != winner && p->score().total_score() == winner->score().total_score())
        {
            tieGame = true;
            break;
        }
    }

    if (tieGame)
    {
        display_alert(4000, "Tie game!", nullptr, [&]()
        {

            set_state(&game::state_game_menu);
        });
    }
    else if (winner == this_player())
    {
        display_alert(4000, "You win!", nullptr, [&]()
        {

            set_state(&game::state_game_menu);
        });
    }
    else
    {
        char buf[BUFSIZ + 1] = {0};
        snprintf(buf, BUFSIZ, "%s wins with %u points!", winner->name().c_str(), winner->score().total_score());
        display_alert(4000, buf, nullptr, [&]()
        {
            set_state(&game::state_game_menu);
        });
    }
}
