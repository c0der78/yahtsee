#include "player.h"
#include <coda/string/util.h>
#include <random>

using namespace coda;
using namespace std;

namespace yahtsee {

    Player::Engine playerEngine;

    die::value_type Player::Engine::generate(die::value_type from, die::value_type to) {
        die::value_type value;

        if (!nextRoll_.empty()) {
            value = nextRoll_.front();

            nextRoll_.pop();
        } else {
            static std::default_random_engine random_engine(time(0));

            uniform_int_distribution<die::value_type> distribution(from, to);

            return distribution(random_engine);
        }
        return value;
    }

    void Player::Engine::reset() {
        while (!nextRoll_.empty()) {
            nextRoll_.pop();
        }
    }

    void Player::Engine::set_next_roll(const queue<die::value_type> &roll) {
        playerEngine.nextRoll_ = roll;
    }

    Player::Player(const string &name)
            : yaht::player(&playerEngine), connection_(NULL), id_(coda::uuid::generate()), name_(name) {
    }

    Player::Player(Connection *conn, const string &id, const string &name)
            : yaht::player(&playerEngine), connection_(conn), id_(id), name_(name) {
    }

    Player::Player(Connection *conn, const Packet &packet) : yaht::player(&playerEngine) {
        from_packet(packet);
    }

    Player::Player(const Player &other)
            : yaht::player(other), connection_(other.connection_), id_(other.id_), name_(other.name_) {
    }

    Player::Player(Player &&other)
            : yaht::player(std::move(other)),
              connection_(other.connection_),
              id_(std::move(other.id_)),
              name_(std::move(other.name_)) {
    }

    Player &Player::operator=(const Player &other) {
        yaht::player::operator=(other);
        connection_ = other.connection_;
        id_ = other.id_;
        name_ = other.name_;

        return *this;
    }

    Player &Player::operator=(Player &&other) {
        yaht::player::operator=(std::move(other));
        connection_ = other.connection_;
        id_ = std::move(other.id_);
        name_ = std::move(other.name_);
        return *this;
    }

    Player::~Player() {
        connection_ = NULL;
    }

    Connection *Player::connection() const {
        return connection_;
    }

    string Player::id() const {
        return id_;
    }

    string Player::name() const {
        return name_;
    }

    void Player::from_packet(const Packet &packet) {
        id_ = packet["id"];
        name_ = packet["name"];
    }

    Player::Packet Player::to_packet() const {
        Packet packet;

        packet["id"] = id_;
        packet["name"] = name_;

        return packet;
    }

    bool Player::operator==(const Player &other) const {
        return id_ == other.id_;
    }

}