#include "player.h"
#include <arg3/str_util.h>
#include <random>

using namespace arg3;

player::engine player_engine;

die::value_type player::engine::generate(die::value_type from, die::value_type to)
{
    die::value_type value;

    if (!nextRoll_.empty())
    {
        value = nextRoll_.back();

        nextRoll_.pop_back();
    }
    else
    {
        static std::default_random_engine random_engine(time(0));

        uniform_int_distribution<die::value_type> distribution(from, to);

        return distribution(random_engine);
    }
    return value;
}

player::player(const string &name) : yaht::player(&player_engine), connection_(NULL), id_(generate_uuid()), name_(name)
{}

player::player(connection *conn, const string &id, const string &name) : yaht::player(), connection_(conn), id_(id), name_(name)
{
}

player::player(connection *conn, const json::object &json) : yaht::player()
{
    from_json(json);
}

connection *player::c0nnection() const
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

void player::from_json(const json::object &json)
{
    id_ = json.get_string("id");
    name_ = json.get_string("name");
}

json::object player::to_json() const
{
    json::object json;

    json.set_string("id", id_);
    json.set_string("name", name_);

    return json;
}

bool player::operator==(const player &other) const
{
    return id_ == other.id_;
}

void player::set_next_roll(const vector<die::value_type> &roll)
{
    player_engine.nextRoll_ = roll;
}