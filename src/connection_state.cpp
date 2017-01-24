#include "client.h"
#include "game.h"
#include "log.h"
#include "player.h"

using namespace rj;

//! the connection has recieved an init packet
void connection::handle_connection_init(const json &packet)
{
    vector<json> players = packet["players"];

    for (const json &player : players) {
        game_->add_player(make_shared<::player>(this, player));
    }

    game_->action_joined_game();
}

//! the connection has recieved a game start packet
void connection::handle_game_start(const json &packet)
{
    string id = packet["start_id"];

    auto player = game_->find_player_by_id(id);

    if (player != nullptr) {
        game_->set_current_player(player);
    }

    log_trace("starting game");

    // set some display flags

    game_->set_needs_display();

    game_->set_needs_clear();

    // and the state

    game_->set_state(&game::state_playing);

    game_->display_alert(2000, "It is now " + game_->current_player()->name() + "'s turn.");
}

//! handle a recieved player roll packet
void connection::handle_player_roll(const packet_format &packet)
{
    string id = packet["player_id"];

    auto p = game_->find_player_by_id(id);

    if (p == nullptr) {
        return;
    }

    vector<packet_format> roll = packet["roll"];

    queue<rj::die::value_type> values;

    for (size_t i = 0; i < roll.size(); i++) {
        auto inner = roll[i];

        auto kept = inner["kept"];

        if (!kept) {
            values.push(inner["value"]);
        }

        p->keep_die(i, kept);
    }

    if (game_->current_player() != p) {
        game_->set_current_player(p);
    }

    player_engine.set_next_roll(values);

    p->roll();

    game_->pop_alert();

    game_->display_dice_roll();

    game_->set_needs_display();
}

//! handle when another player has joined the game
void connection::handle_remote_connection_init(const json &packet)
{
    game_->action_add_network_player(make_shared<player>(this, packet));
}

void connection::handle_player_joined(const packet_format &packet)
{
    packet_format player = packet["player"];

    string id = player["id"];

    if (id != game_->this_player()->id()) {
        string name = player["name"];

        log_trace("new player found (%s)", name.c_str());

        auto p = make_shared<::player>(this, player);

        game_->action_network_player_joined(p);
    }
}

void connection::handle_player_left(const packet_format &packet)
{
    packet_format player = packet["player"];

    string id = player["id"];

    if (id != game_->this_player()->id()) {
        auto p = game_->find_player_by_id(id);

        if (p != nullptr) {
            game_->action_network_player_left(p);
        }
    }
}

void connection::handle_player_turn_finished(const packet_format &packet)
{
    string id = packet["player_id"];

    auto player = game_->find_player_by_id(id);

    if (player == nullptr) {
        log_trace("turn finish: player %s not found", player->id().c_str());
        return;
    }

    vector<packet_format> upper = packet["upper"];

    for (auto i = 0; i < upper.size(); i++) {
        int value = upper[i];

        if (value == -1) {
            player->score().upper_score(i + 1, 0, false);
        } else {
            player->score().upper_score(i + 1, value);
        }
    }

    vector<packet_format> lower = packet["lower"];

    for (auto i = 0; i < lower.size(); i++) {
        yaht::scoresheet::type type = static_cast<yaht::scoresheet::type>(i);

        int value = lower[i];

        if (value < 0) {
            player->score().lower_score(type, 0, false);
        } else {
            player->score().lower_score(type, value);
        }
    }

    game_->action_network_player_finished(player);
}
