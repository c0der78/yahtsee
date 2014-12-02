#include "client.h"
#include "player.h"
#include <arg3json/json.h>
#include "game.h"
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

    game_->for_players([&players](const shared_ptr<player> &p)
    {
        players.add(p->to_json());
    });

    packet.set_array("players", players);

    logf("writing new connection %s", packet.to_string().c_str());

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

    logf("recieved packet %s", packet.to_string().c_str());

    client_action action = (client_action) packet.get_int("action");

    switch (action)
    {
    case CLIENT_INIT:
    {
        string name = packet.get_string("name");
        string id = packet.get_string("id");

        logf("action client init");

        game_->action_add_network_player(make_shared<player>(this, id, name));

        break;
    }
    case PLAYER_JOINED:
    {
        json::object player = packet.get("player");

        string id = player.get_string("id");

        if (id != game_->this_player()->id())
        {
            string name = player.get_string("name");

            auto p = make_shared<::player>(this, player);

            game_->action_network_player_joined(p);

            logf("action player joined");
        }
        break;
    }
    case PLAYER_LEFT:
    {
        json::object player = packet.get("player");

        string id = player.get_string("id");

        if (id != game_->this_player()->id())
        {
            auto p = game_->find_player_by_id(id);

            if (p != nullptr)
                game_->action_network_player_left(p);

            logf("action player left");
        }

        break;
    }
    /* connection is someone connect to the server */
    case CONNECTION_INIT:
    {
        logf("action connection init");

        json::array players = packet.get_array("players");

        for (const json::object &player : players)
        {
            game_->add_player(make_shared<::player>(this, player));
        }

        game_->action_joined_game();

        break;
    }
    case GAME_START:
    {
        string id = packet.get_string("start_id");

        auto player = game_->find_player_by_id(id);

        if (player != nullptr)
            game_->set_current_player(player);

        game_->set_state(&game::state_playing);

        game_->refresh(true);

        logf("action game started");

        break;
    }
    }
}

void connection::on_will_write()
{
}

void connection::on_did_write()
{
    logf("wrote to socket");
}

connection_factory::connection_factory(game *game) : game_(game)
{
}

std::shared_ptr<buffered_socket> connection_factory::create_socket(socket_server *server, SOCKET sock, const sockaddr_storage &addr)
{
    auto socket = make_shared<connection>(game_, sock, addr);

    connections_.push_back(socket);

    return socket;
}

void connection_factory::for_connections(std::function<void(const shared_ptr<connection> &conn)> funk)
{
    for (const auto &c : connections_)
    {
        funk(c);
    }
}

client::client(game *game, SOCKET sock, const sockaddr_storage &addr) : connection(game, sock, addr), backgroundThread_(nullptr)
{}

client::client(game *game) : connection(game), backgroundThread_(nullptr)
{
}

client::client(client &&other) : connection(std::move(other)), backgroundThread_(std::move(other.backgroundThread_))
{}

client::~client()
{
    if (backgroundThread_ != nullptr)
    {
        backgroundThread_->join();
        backgroundThread_ = nullptr;
    }
}

client &client::operator=(client && other)
{
    connection::operator=(std::move(other));

    backgroundThread_ = std::move(other.backgroundThread_);

    return *this;
}

void client::on_connect()
{
    json::object packet;

    packet.set_int("action", CLIENT_INIT);

    packet.set_string("name", game_->this_player()->name());

    packet.set_string("id", game_->this_player()->id());

    writeln(packet.to_string());
}

void client::on_close()
{
    game_->action_disconnected();
}

bool client::start(const std::string &host, int port)
{
    if (!connect(host, port))
        return false;

    run();

    return true;
}

bool client::start_in_background(const std::string &host, int port)
{
    if (!connect(host, port))
        return false;

    set_non_blocking(true);

    backgroundThread_ = make_shared<thread>(&client::run, this);

    return true;
}

void client::run()
{
    std::chrono::milliseconds dura( 200 );

    while (is_valid())
    {
        if (!read_to_buffer())
        {
            close();
            break;
        }

        if (!write_from_buffer())
        {
            close();
            break;
        }

        std::this_thread::sleep_for( dura );
    }

    logf("client finished.");
}
