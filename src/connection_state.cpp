#include "connection_state.h"

#include <rj/log/log.h>
#include <vector>

#include "client.h"
#include "game.h"
#include "player.h"

using namespace rj;

namespace yahtsee {

    ConnectionState::ConnectionState(StateManager *state) :
            state_(state) {}

    void ConnectionState::handle(Connection *conn, const Packet &packet) {

        ClientAction action = static_cast<ClientAction>(packet["action"].get<int>());

        switch (action) {
            case ClientAction::INIT: {
                if (static_cast<Client *>(conn)) {
                    log::trace("handling remote connect init");
                    handle_remote_connection_init(conn, packet);
                } else {
                    log::trace("handling connection init");
                    handle_connection_init(conn, packet);
                }
                break;
            }
            case ClientAction::PLAYER_JOINED: {
                log::trace("handling player joined");
                handle_player_joined(conn, packet);
                break;
            }
            case ClientAction::PLAYER_LEFT: {
                log::trace("handling player left");
                handle_player_left(conn, packet);
                break;
            }
            case ClientAction::PLAYER_ROLL: {
                log::trace("handling player roll");
                handle_player_roll(conn, packet);
                break;
            }
            case ClientAction::START: {
                log::trace("handling game start");
                handle_game_start(conn, packet);
                break;
            }
            case ClientAction::PLAYER_TURN_FINISHED: {
                log::trace("handling player turn finished");
                handle_player_turn_finished(conn, packet);
                break;
            }
            default: {
                log::trace("unknown action for packet");
                break;
            }
        }
    }

    //! the connection has recieved an init packet
    void ConnectionState::handle_connection_init(Connection *conn, const Packet &packet) {
        std::vector <Packet> players = packet["players"];

        for (const Packet &player : players) {
            state_->players()->add(std::make_shared<Player>(conn, player));
        }

        state_->events()->joined_game();
    }

    //! the connection has recieved a game start packet
    void ConnectionState::handle_game_start(Connection *conn, const Packet &packet) {
        std::string id = packet["start_id"];

        auto player = state_->players()->find_by_id(id);

        if (player != nullptr) {
            state_->players()->set_turn(player);
        }

        log::trace("starting game");

        state_->ui()->set_needs_refresh();

        state_->ui()->flash("It is now " + state_->players()->turn()->name() + "'s turn.");
    }

    //! handle a recieved player roll packet
    void ConnectionState::handle_player_roll(Connection *conn, const Packet &packet) {
        std::string id = packet["player_id"];

        auto p = state_->players()->find_by_id(id);

        if (p == nullptr) {
            return;
        }

        std::vector <Packet> roll = packet["roll"];

        std::queue <rj::die::value_type> values;

        for (size_t i = 0; i < roll.size(); i++) {
            auto inner = roll[i];

            auto kept = inner["kept"];

            if (!kept) {
                values.push(inner["value"]);
            }

            p->keep_die(i, kept);
        }

        if (state_->players()->turn() != p) {
            state_->players()->set_turn(p);
        }

        playerEngine.set_next_roll(values);

        p->roll();

        state_->ui()->dice_roll();

        state_->ui()->set_needs_refresh();
    }

    //! handle when another player has joined the game
    void ConnectionState::handle_remote_connection_init(Connection *conn, const Packet &packet) {
        state_->events()->add_network_player(std::make_shared<Player>(conn, packet));
    }

    void ConnectionState::handle_player_joined(Connection *conn, const Packet &packet) {
        Packet player = packet["player"];

        std::string id = player["id"];

        if (id != state_->players()->self()->id()) {
            std::string name = player["name"];

            log::trace("new player found (", name.c_str(), ")");

            auto p = std::make_shared<Player>(conn, player);

            state_->events()->network_player_joined(p);
        }
    }

    void ConnectionState::handle_player_left(Connection *conn, const Packet &packet) {
        Packet player = packet["player"];

        std::string id = player["id"];

        if (id != state_->players()->self()->id()) {
            auto p = state_->players()->find_by_id(id);

            if (p != nullptr) {
                state_->events()->network_player_left(p);
            }
        }
    }

    void ConnectionState::handle_player_turn_finished(Connection *conn, const Packet &packet) {
        std::string id = packet["player_id"];

        auto player = state_->players()->find_by_id(id);

        if (player == nullptr) {
            log::trace("turn finish: player ", id.c_str(), " not found");
            return;
        }

        std::vector <Packet> upper = packet["upper"];

        for (auto i = 0; i < upper.size(); i++) {
            int value = upper[i];

            if (value == -1) {
                player->score().upper_score(i + 1, 0, false);
            } else {
                player->score().upper_score(i + 1, value);
            }
        }

        std::vector <Packet> lower = packet["lower"];

        for (auto i = 0; i < lower.size(); i++) {
            yaht::scoresheet::type type = static_cast<yaht::scoresheet::type>(i);

            int value = lower[i];

            if (value < 0) {
                player->score().lower_score(type, 0, false);
            } else {
                player->score().lower_score(type, value);
            }
        }

        state_->events()->network_player_finished(player);
    }
}
