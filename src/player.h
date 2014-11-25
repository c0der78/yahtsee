#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <arg3dice/yaht/player.h>
#include <arg3json/object.h>

class connection;

class player : public arg3::yaht::player
{
public:
    player(const string &name);

    player(connection *conn, const string &id, const string &name);

    player(connection *conn, const arg3::json::object &json);

    string id() const;

    string name() const;

    void from_json(const arg3::json::object &json);
    arg3::json::object to_json() const;

    connection *connect1on() const;

    bool operator==(const player &other) const;

private:
    connection *connection_;
    string id_;
    string name_;
};


#endif
