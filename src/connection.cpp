
#include "client.h"
#include "game.h"
#include <rj/log/log.h>
#include "player.h"

using namespace rj;
using namespace rj::net;

namespace yahtsee {

Connection::Connection(SOCKET sock, const sockaddr_storage &addr) : buffered_socket(sock, addr)
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
    packet["action"] = CONNECTION_INIT;

    std::vector<Packet> playerPackets;

    for (const Player &player : players) {
        playerPackets.push_back(player.to_packet());
    }

    packet["players"] = playerPackets;

    log::trace("connection connected, sending ", packet.dump());

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

        log::trace("recieved ", packet.dump());

        // validate the packet
        // TODO: verify a game/session id?
        if (packet.find("action") == packet.end()) {
            throw runtime_error("packet has no action");
        }

        ClientAction action = static_cast<ClientAction>(packet["action"].get<int>());

        switch (action) {
            case CONNECTION_INIT: {
                if (static_cast<Client*>(this)) {
                    log::trace("handling remote connect init");
                    handle_remote_connection_init(packet);
                } else {
                    log::trace("handling connection init");
                    handle_connection_init(packet);
                }
                break;
            }
            case PLAYER_JOINED: {
                log::trace("handling player joined");
                handle_player_joined(packet);
                break;
            }
            case PLAYER_LEFT: {
                log::trace("handling player left");
                handle_player_left(packet);
                break;
            }
            case PLAYER_ROLL: {
                log::trace("handling player roll");
                handle_player_roll(packet);
                break;
            }
            case GAME_START: {
                log::trace("handling game start");
                handle_game_start(packet);
                break;
            }
            case PLAYER_TURN_FINISHED: {
                log::trace("handling player turn finished");
                handle_player_turn_finished(packet);
                break;
            }
            default: {
                log::trace("unknown action for packet");
                break;
            }
        }
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