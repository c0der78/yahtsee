#include "client.h"
#include "game.h"
#include "player.h"
#include "log.h"

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

    game_->for_players([&](const shared_ptr<player> &p)
    {
        if (p->c0nnection() != this) {
            players.add(p->to_json());
        }

        return false;
    });

    packet.set_array("players", players);

    writeln(packet.to_string());

    logf("connection connected, sending %s", packet.to_string().c_str());
}

void connection::on_close()
{
    logf("connection closed");

    game_->action_remove_network_player(this);
}

void connection::on_will_read()
{
}

void connection::on_did_read()
{
    json::object packet;

    packet.parse(readln());

    logf("recieved %s", packet.to_string().c_str());

    if (!packet.contains("action")) {
        throw runtime_error("packet has no action");
    }

    client_action action = (client_action) packet.get_int("action");

    switch (action)
    {
    case REMOTE_CONNECTION_INIT:
    {
        logf("handling remote connect init");
        handle_remote_connection_init(packet);
        break;
    }
    case PLAYER_JOINED:
    {
        logf("handling player joined");
        handle_player_joined(packet);
        break;
    }
    case PLAYER_LEFT:
    {
        logf("handling player left");
        handle_player_left(packet);
        break;
    }
    case PLAYER_ROLL:
    {
        logf("handling player roll");
        handle_player_roll(packet);
        break;
    }
    /* connection is someone connect to the server */
    case CONNECTION_INIT:
    {
        logf("handling connection init");
        handle_connection_init(packet);
        break;
    }
    case GAME_START:
    {
        logf("handling game start");
        handle_game_start(packet);
        break;
    }
    case PLAYER_TURN_FINISHED:
    {
        logf("handling player turn finished");
        handle_player_turn_finished(packet);
        break;
    }
    default:
    {
        logf("unknown action for packet");
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
