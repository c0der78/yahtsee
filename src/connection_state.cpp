#include "client.h"
#include "game.h"
#include "player.h"
#include "log.h"

using namespace arg3;

void connection::handle_connection_init(const json::object &packet)
{
    json::array players = packet.get_array("players");

    for (const json::object &player : players)
    {
        game_->add_player(make_shared<::player>(this, player));
    }

    game_->action_joined_game();
}

void connection::handle_game_start(const json::object &packet)
{
    string id = packet.get_string("start_id");

    auto player = game_->find_player_by_id(id);

    if (player != nullptr)
        game_->set_current_player(player);

    logf("starting game");

    game_->set_state(&game::state_playing);

    game_->set_needs_display();

    game_->set_needs_clear();
}

void connection::handle_player_roll(const json::object &packet)
{
    string id = packet.get_string("player_id");

    auto p = game_->find_player_by_id(id);

    if (p == nullptr) return;

    json::array roll = packet.get_array("roll");

    queue<arg3::die::value_type> values;

    for (size_t i = 0; i < roll.size(); i++)
    {
        json::object inner = roll.get(i);

        values.push(inner.get_int("value"));

        p->keep_die(i, inner.get_bool("kept"));
    }

    if (game_->current_player() != p)
        game_->set_current_player(p);

    player_engine.set_next_roll(values);

    p->roll();

    game_->pop_alert();

    game_->display_dice_roll();

}
void connection::handle_remote_connection_init(const json::object &packet)
{

    game_->action_add_network_player(make_shared<player>(this, packet));
}

void connection::handle_player_joined(const json::object &packet)
{
    json::object player = packet.get("player");

    string id = player.get_string("id");

    if (id != game_->this_player()->id())
    {
        string name = player.get_string("name");

        auto p = make_shared<::player>(this, player);

        game_->action_network_player_joined(p);
    }
}

void connection::handle_player_left(const json::object &packet)
{
    json::object player = packet.get("player");

    string id = player.get_string("id");

    if (id != game_->this_player()->id())
    {
        auto p = game_->find_player_by_id(id);

        if (p != nullptr)
            game_->action_network_player_left(p);
    }
}

void connection::handle_player_turn_finished(const json::object &packet)
{
    string id = packet.get_string("player_id");

    auto player = game_->find_player_by_id(id);

    if (player == nullptr)
    {
        logf("turn finish: player %s not found", player->id().c_str());
        return;
    }

    json::array upper = packet.get_array("upper");

    for (auto i = 0; i < upper.size(); i++)
    {
        player->score().upper_score(i + 1, upper.get_int(i));

    }

    json::array lower = packet.get_array("lower");

    for (auto i = 0; i < lower.size(); i++)
    {
        yaht::scoresheet::type type = static_cast<yaht::scoresheet::type>(i);

        player->score().lower_score(type, lower.get_int(i));
    }

    game_->action_network_player_finished(player);
}



