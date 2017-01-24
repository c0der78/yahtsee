#include "player.h"
#include <rj/string/util.h>
#include <random>

using namespace rj;

player::engine player_engine;

die::value_type player::engine::generate(die::value_type from, die::value_type to)
{
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

void player::engine::reset()
{
    while (!nextRoll_.empty()) {
        nextRoll_.pop();
    }
}

void player::engine::set_next_roll(const queue<die::value_type> &roll)
{
    player_engine.nextRoll_ = roll;
}

player::player(const string &name)
    : yaht::player(&player_engine), connection_(NULL), id_(rj::uuid::generate()), name_(name)
{
}

player::player(connection *conn, const string &id, const string &name)
    : yaht::player(&player_engine), connection_(conn), id_(id), name_(name)
{
}

player::player(connection *conn, const packet_format &packet) : yaht::player(&player_engine)
{
    from_packet(packet);
}

player::player(const player &other)
    : yaht::player(other), connection_(other.connection_), id_(other.id_), name_(other.name_)
{
}

player::player(player &&other)
    : yaht::player(std::move(other)),
      connection_(other.connection_),
      id_(std::move(other.id_)),
      name_(std::move(other.name_))
{
}

player &player::operator=(const player &other)
{
    yaht::player::operator=(other);
    connection_ = other.connection_;
    id_ = other.id_;
    name_ = other.name_;

    return *this;
}

player &player::operator=(player &&other)
{
    yaht::player::operator=(std::move(other));
    connection_ = other.connection_;
    id_ = std::move(other.id_);
    name_ = std::move(other.name_);
    return *this;
}

player::~player()
{
    connection_ = NULL;
}

connection *player::get_connection() const
{
    return connection_;
}

string player::id() const
{
    return id_;
}

string player::name() const
{
    return name_;
}

void player::from_packet(const packet_format &packet)
{
    id_ = packet["id"];
    name_ = packet["name"];
}

player::packet_format player::to_packet() const
{
    packet_format packet;

    packet["id"] = id_;
    packet["name"] = name_;

    return packet;
}

bool player::operator==(const player &other) const
{
    return id_ == other.id_;
}
