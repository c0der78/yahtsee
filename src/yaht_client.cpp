#include "yaht_client.h"
#include <arg3dice/yaht/engine.h>
#include <arg3json/object.h>
#include "yaht_game.h"

using namespace arg3;
using namespace arg3::net;

yaht_client::yaht_client(yaht_game *game, SOCKET sock, const sockaddr_in &addr) : buffered_socket(sock, addr), game_(game)
{}

yaht_client::yaht_client(yaht_game *game) : buffered_socket(), game_(game)
{
}

yaht_client_factory::yaht_client_factory(yaht_game *game) : game_(game)
{
}

void yaht_client::on_connect()
{
    json::object packet;

    packet.set_int("action", CLIENT_INIT);
    packet.set_string("name", (*yaht::engine::instance())[0]->name());

    write(packet);
}

void yaht_client::on_close()
{
    // game_->action_remove_network_player( );
}

void yaht_client::on_will_read()
{
    //log::trace(format("{0} will read", sock->getIP()));
}

void yaht_client::on_did_read()
{
    json::object packet = json::object(readln());

    client_action action = (client_action) packet.get_int("action");

    switch (action)
    {
    case CLIENT_INIT:
    {
        string name = packet.get_string("name");

        game_->action_add_network_player(name);

        break;
    }
    }
}

void yaht_client::on_will_write()
{
    //log::trace(format("{0} will write", sock->getIP()));
}

void yaht_client::on_did_write()
{
}


std::shared_ptr<buffered_socket> yaht_client_factory::create_socket(socket_server *server, SOCKET sock, const sockaddr_in &addr)
{
    auto socket = make_shared<yaht_client>(game_, sock, addr);

    return socket;
}
