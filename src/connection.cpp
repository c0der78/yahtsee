#include "client.h"
#include "game.h"
#include "player.h"

using namespace arg3;
using namespace arg3::net;

connection::connection(game *game, SOCKET sock, const sockaddr_storage &addr) : buffered_socket(sock, addr), game_(game)
{}

connection::connection(game *game) : buffered_socket(), game_(game)
{
}

connection::connection(connection &&other) : buffered_socket(std::move(other)), game_(other.game_)
{}

connection::~connection()
{
}

connection &connection::operator=(connection && other)
{
    buffered_socket::operator=(std::move(other));

    game_ = other.game_;

    return *this;
}

void connection::on_connect()
{
    json::object packet;

    packet.set_int("action", CONNECTION_INIT);

    json::array players;

    game_->for_players([&players](const shared_ptr<player> &p)
    {
        players.add(p->to_json());
    });

    packet.set_array("players", players);

    writeln(packet.to_string());
}

void connection::on_close()
{
    game_->action_remove_network_player(this);
}

void connection::on_will_read()
{
}

void connection::on_did_read()
{
    json::object packet;

    packet.parse(readln());

    client_action action = (client_action) packet.get_int("action");

    switch (action)
    {
    case REMOTE_CONNECTION_INIT:
    {
        handle_remote_connection_init(packet);
        break;
    }
    case PLAYER_JOINED:
    {
        handle_player_joined(packet);
        break;
    }
    case PLAYER_LEFT:
    {
        handle_player_left(packet);
        break;
    }
    case PLAYER_ROLL:
    {
        handle_player_roll(packet);
        break;
    }
    /* connection is someone connect to the server */
    case CONNECTION_INIT:
    {
        handle_connection_init(packet);
        break;
    }
    case GAME_START:
    {
        handle_game_start(packet);
        break;
    }
    case PLAYER_TURN_FINISHED:
    {
        handle_player_turn_finished(packet);
        break;
    }
    }
}

void connection::on_will_write()
{
}

void connection::on_did_write()
{
}
