#include "game.h"
#include "player.h"

using namespace arg3;

void game::action_host_game()
{
    display_alert("Starting server...");

    string error;

    bool response = matchmaker_.host(&error);

    pop_alert(); // done registration

    if (!response)
    {
        display_alert({ "Unable to register game at this time.", error } );

        pop_alert(2000, [&]()
        {
            set_state(&game::state_multiplayer_menu);
            display_multiplayer_menu();
        });

        players_.clear();

        flags_ = 0;

        return;
    }

    set_state(&game::state_waiting_for_connections);

    display_alert("Waiting for connections...");
}

void game::action_join_game()
{
    display_alert("Finding game to join...");

    string error;

    bool result = matchmaker_.join_best_game(&error);

    pop_alert();

    if (!result)
    {
        display_alert({"Unable to find game to join at this time.", error});

        pop_alert(2000, [&]()
        {
            set_state(&game::state_multiplayer_menu);
            display_multiplayer_menu();
        });

        players_.clear();

        flags_ = 0;

        return;
    }
}

void game::action_joined_game()
{
    pop_alert();

    display_client_waiting_to_start();
}

void game::action_disconnected()
{
    if (!alive()) return;

    clear_alerts();

    display_alert("Disconnected from server.");

    pop_alert(2000, [&]()
    {
        set_state(&game::state_game_menu);

        display_game_menu();
    });
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
        return p->connect1on() == c;
    });

    if (it != players_.end())
    {
        auto p = *it;

        players_.erase(it);

        if (players_.size() == 0)
        {
            players_.clear();

            display_alert(p->name() + " has left the game.");

            pop_alert(2000, [&]()
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
    auto it = remove_if(players_.begin(), players_.end(), [&p](const shared_ptr<player> &o)
    {
        return p->id() == o->id();
    });

    if (it == players_.end()) return;

    players_.erase(it, players_.end());

    char buf[BUFSIZ + 1] = {0};

    snprintf(buf, BUFSIZ, "%s has left the game.", p->name().c_str());

    display_alert(2000, buf);
}


void game::action_roll_dice()
{
    auto player = current_player();

    if (player->id() != this_player()->id())
    {
        display_alert(2000, "It is not your turn.");
        return;
    }

    if (player->roll_count() < 3)
    {
        player->roll();

        display_dice_roll();

        matchmaker_.notify_player_roll();
    }
    else
    {
        display_alert(2000, "You must choose a score after three rolls.");
    }
}

void game::action_finish_turn()
{
    set_state(&game::state_playing);

    next_player();

    refresh(true);
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
    if (!player->score().lower_score(type))
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
    if (!player->score().upper_score(n))
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

        if (!player->score().upper_score(best_upper.first))
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
        if (!player->score().lower_score(best_lower.first))
        {
            player->score().lower_score(best_lower.first, best_lower.second);

            action_finish_turn();
        }
        else
        {
            display_already_scored();
        }
    }
    else
    {
        display_alert(2000, "No best score found!");
    }
}
