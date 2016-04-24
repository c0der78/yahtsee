#include "client.h"
#include <arg3str/util.h>
#include "game.h"
#include "log.h"
#include "player.h"

using namespace arg3;
using namespace arg3::net;

/* connection factory */
connection_factory::connection_factory(game *game) : game_(game)
{
}

std::shared_ptr<buffered_socket> connection_factory::create_socket(const server_type &server, SOCKET sock, const sockaddr_storage &addr)
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

void client::on_will_write()
{
    log_trace("client will write");
}

void client::on_did_write()
{
    log_trace("client did write");
}

void client::on_will_read()
{
    // logstr("client will read");
}

void client::on_did_read()
{
    log_trace("client did read");

    connection::on_did_read();
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
            log_trace("unable to read to client buffer (%d: %s)", errno, strerror(errno));
            close();
            break;
        }

        // check for error writing
        if (!write_from_buffer()) {
            log_trace("unable to write to client buffer (%d: %s)", errno, strerror(errno));
            close();
            break;
        }

        // give other threads a chance
        std::this_thread::sleep_for(dura);
    }

    log_trace("client finished");
}
