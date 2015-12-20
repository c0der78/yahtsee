#include "client.h"
#include "player.h"
#include "game.h"
#include "log.h"
#include <arg3str/util.h>

using namespace arg3;
using namespace arg3::net;

/* connection factory */
connection_factory::connection_factory(game *game) : game_(game)
{
}

std::shared_ptr<buffered_socket> connection_factory::create_socket(socket_server *server, SOCKET sock, const sockaddr_storage &addr)
{
    auto socket = make_shared<connection>(game_, sock, addr);

    // add to the list of connections
    connections_.push_back(socket);

    return socket;
}

void connection_factory::for_connections(std::function<void(const shared_ptr<connection> &conn)> funk)
{
    for (const auto &c : connections_) {
        funk(c);
    }
}

/*  client */
client::client(game *game, SOCKET sock, const sockaddr_storage &addr) : connection(game, sock, addr), backgroundThread_(nullptr)
{
}

client::client(game *game) : connection(game), backgroundThread_(nullptr)
{
}

client::client(client &&other) : connection(std::move(other)), backgroundThread_(std::move(other.backgroundThread_))
{
}

client::~client()
{
    log_trace("destroying client");
    if (backgroundThread_ != nullptr) {
        backgroundThread_->join();
        backgroundThread_ = nullptr;
    }
}

client &client::operator=(client &&other)
{
    connection::operator=(std::move(other));

    backgroundThread_ = std::move(other.backgroundThread_);

    return *this;
}

//! handle when connected
void client::on_connect()
{
    json::object packet;

    packet.set_int("action", REMOTE_CONNECTION_INIT);

    packet.set_string("name", game_->this_player()->name());

    packet.set_string("id", game_->this_player()->id());

    writeln(packet.to_string());

    log_trace("client connected, sending %s", packet.to_string().c_str());
}

//! handle when closed
void client::on_close()
{
    log_trace("client closed");
}

//! start the client for a give host:port
bool client::start(const std::string &host, int port)
{
    if (!connect(host, port)) {
        return false;
    }

    set_non_blocking(true);

    run();

    return true;
}

//! start the client in the background for a given host:port
bool client::start_in_background(const std::string &host, int port)
{
    if (!connect(host, port)) {
        return false;
    }

    set_non_blocking(true);

    backgroundThread_ = make_shared<thread>(&client::run, this);

    return true;
}

//! the magic run method
void client::run()
{
    std::chrono::milliseconds dura(200);

    log_trace("client starting");

    while (is_valid()) {
        // check for error reading
        if (!read_to_buffer()) {
            log_trace("unable to read to client buffer");
            close();
            break;
        } else if (input().size() > 0) {
            string tempIn(input().begin(), input().end());
            trim(tempIn);
            log_trace("read %zu bytes %s", input().size(), tempIn.c_str());
        }

        size_t outSize = output().size();

        // check for error writing
        if (!write_from_buffer()) {
            log_trace("unable to write to client buffer");
            close();
            break;
        } else if (outSize > 0) {
            log_trace("wrote %zu bytes", outSize);
        }

        // give other threads a chance
        std::this_thread::sleep_for(dura);
    }

    log_trace("client finished");
}
