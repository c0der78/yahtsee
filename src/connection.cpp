#include "connection.h"
#include "client.h"
#include "game.h"
#include <coda/log/log.h>
#include "player.h"
#include "connection_state.h"

using namespace coda;
using namespace coda::net;

namespace yahtsee {

Connection::Connection(SOCKET sock, const sockaddr_storage &addr, const std::shared_ptr<ConnectionState> &state)
        : buffered_socket(sock, addr), state_(state)
{
}

Connection::Connection() : buffered_socket()
{
}

Connection::Connection(Connection &&other) : buffered_socket(std::move(other))
{
}

Connection::~Connection()
{
    log::trace("destroying connection");
}

Connection &Connection::operator=(Connection &&other)
{
    buffered_socket::operator=(std::move(other));

    return *this;
}

//! handle when connected
void Connection::on_connect(const std::vector<Player> &players)
{
    Packet packet;

    // build the init packet
    packet["action"] = static_cast<int>(ClientAction::INIT);

    std::vector<Packet> playerPackets;

    for (const Player &player : players) {
        playerPackets.push_back(player.to_packet());
    }

    packet["players"] = playerPackets;

    log::trace("connection connected, sending ", packet.dump().c_str());

    writeln(packet);
}

//! handle when closed
void Connection::on_close()
{
    log::trace("connection closed (", errno, ": ", strerror(errno), ")");
}

void Connection::on_will_read()
{
    // logstr("connection will read");
}

//! handle when data is read
void Connection::on_did_read()
{
    Packet packet;

    while (has_input()) {
        // read a packet
        if (!packet.parse(readln())) {
            log::trace("unable to parse line from buffer");
            return;
        }

        log::trace("recieved ", packet.dump().c_str());

        // validate the packet
        // TODO: verify a game/session id?
        if (packet.find("action") == packet.end()) {
            throw runtime_error("packet has no action");
        }

        state_->handle(this, packet);
    }
}

void Connection::on_will_write()
{
    log::trace("connection will write");
}

void Connection::on_did_write()
{
    log::trace("connection did write");
}

}