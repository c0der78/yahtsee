#include "yaht_client.h"
#include <arg3dice/yaht/engine.h>
#include <arg3json/object.h>
#include "yaht_game.h"

using namespace arg3;
using namespace arg3::net;

yaht_client::yaht_client(yaht_game *game, SOCKET sock, const sockaddr_storage &addr) : buffered_socket(sock, addr), backgroundThread_(nullptr), game_(game)
{}

yaht_client::yaht_client(yaht_game *game) : buffered_socket(), backgroundThread_(nullptr), game_(game)
{
}

yaht_client::yaht_client(yaht_client &&other) : buffered_socket(std::move(other)), backgroundThread_(std::move(other.backgroundThread_)),
    game_(other.game_)
{}

yaht_client::~yaht_client()
{
    if (backgroundThread_ != nullptr)
    {
        if (is_valid())
            close();

        backgroundThread_->join();
        backgroundThread_ = nullptr;
    }
}

yaht_client &yaht_client::operator=(yaht_client && other)
{
    backgroundThread_ = std::move(other.backgroundThread_);
    game_ = other.game_;

    return *this;
}

void yaht_client::on_connect()
{
    json::object packet;

    packet.set_int("action", CLIENT_INIT);
    packet.set_string("name", (*yaht::engine::instance())[0]->name());

    writeln(packet.to_string());
}

void yaht_client::on_close()
{
}

void yaht_client::on_will_read()
{
}

void yaht_client::on_did_read()
{
    json::object packet;

    packet.parse(readln());

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
}

void yaht_client::on_did_write()
{
}

yaht_client_factory::yaht_client_factory(yaht_game *game) : game_(game)
{
}

std::shared_ptr<buffered_socket> yaht_client_factory::create_socket(socket_server *server, SOCKET sock, const sockaddr_storage &addr)
{
    auto socket = make_shared<yaht_client>(game_, sock, addr);

    return socket;
}

bool yaht_client::start(const std::string &host, int port)
{
    if (!connect(host, port))
        return false;

    run();
    return true;
}

bool yaht_client::start_in_background(const std::string &host, int port)
{
    if (!connect(host, port))
        return false;

    set_non_blocking(true);

    backgroundThread_ = make_shared<thread>(&yaht_client::run, this);
    return true;
}

void yaht_client::run()
{
    try
    {
        std::chrono::milliseconds dura( 100 );
        while (is_valid())
        {
            if (!read_to_buffer())
            {
                std::this_thread::sleep_for( dura );
            }

            if (!write_from_buffer())
            {
                std::this_thread::sleep_for( dura );
            }

            std::this_thread::sleep_for( dura );
        }
    }
    catch (...)
    {
        cerr << "caught unknown exception" << endl;
    }
}
