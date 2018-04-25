#include "client.h"
#include <coda/string/util.h>
#include <coda/log/log.h>
#include "game.h"
#include "player.h"

using namespace coda;
using namespace coda::net;

namespace yahtsee {

    /*  client */
    Client::Client(SOCKET sock, const sockaddr_storage &addr, const std::shared_ptr<ConnectionState> &state)
            : Connection(sock, addr, state), backgroundThread_(nullptr) {
    }

    Client::Client() : Connection(), backgroundThread_(nullptr) {
    }

    Client::Client(Client &&other) : Connection(std::move(other)),
                                     backgroundThread_(std::move(other.backgroundThread_)) {
    }

    Client::~Client() {
        log::trace("destroying client");
        if (backgroundThread_ != nullptr) {
            backgroundThread_->join();
            backgroundThread_ = nullptr;
        }
    }

    Client &Client::operator=(Client &&other) {
        Connection::operator=(std::move(other));

        backgroundThread_ = std::move(other.backgroundThread_);

        return *this;
    }

    //! handle when connected
    void Client::on_connect(const std::shared_ptr<Player> &player) {
        Packet packet;

        packet["action"] = static_cast<int>(ClientAction::INIT);

        packet["name"] = player->name();

        packet["id"] = player->id();

        writeln(packet);

        log::trace("client connected, sending ", packet.dump().c_str());
    }

    //! handle when closed
    void Client::on_close() {
        log::trace("client closed");
    }

    void Client::on_will_write() {
        log::trace("client will write");
    }

    void Client::on_did_write() {
        log::trace("client did write");
    }

    void Client::on_will_read() {
        // logstr("client will read");
    }

    void Client::on_did_read() {
        log::trace("client did read");

        Connection::on_did_read();
    }

    //! start the client for a give host:port
    bool Client::start(const std::string &host, int port) {
        if (!connect(host, port)) {
            return false;
        }

        set_non_blocking(true);

        run();

        return true;
    }

    //! start the client in the background for a given host:port
    bool Client::start_in_background(const std::string &host, int port) {
        if (!connect(host, port)) {
            return false;
        }

        set_non_blocking(true);

        backgroundThread_ = make_shared<thread>(&Client::run, this);

        return true;
    }

    //! the magic run method
    void Client::run() {
        std::chrono::milliseconds dura(200);

        log::trace("client starting");

        while (is_valid()) {
            // check for error reading

            if (!read_to_buffer()) {
                log::trace("unable to read to client buffer (", errno, ": ", strerror(errno), ")");
                close();
                break;
            }

            // check for error writing
            if (!write_from_buffer()) {
                log::trace("unable to write to client buffer (", errno, ": ", strerror(errno), ")");
                close();
                break;
            }

            // give other threads a chance
            std::this_thread::sleep_for(dura);
        }

        log::trace("client finished");
    }
}