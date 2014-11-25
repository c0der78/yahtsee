#include "player.h"
#include <arg3/str_util.h>

using namespace arg3;

player::player(const string &name) : yaht::player(), connection_(NULL), id_(generate_uuid()), name_(name)
{}

player::player(connection *conn, const string &id, const string &name) : yaht::player(), connection_(conn), id_(id), name_(name)
{
}

player::player(connection *conn, const json::object &json) : yaht::player()
{
    from_json(json);
}

connection *player::connect1on() const
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
