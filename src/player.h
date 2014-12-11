#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <arg3dice/yaht/player.h>
#include <arg3json/object.h>
#include <queue>

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

    connection *c0nnection() const;

    bool operator==(const player &other) const;

    class engine : public arg3::die::engine
    {
    public:
        arg3::die::value_type generate(arg3::die::value_type from, arg3::die::value_type to);
        void reset();
        void set_next_roll(const queue<arg3::die::value_type> &values);
    private:
        queue<arg3::die::value_type> nextRoll_;
        friend class player;
    };

private:

    connection *connection_;
    string id_;
    string name_;
};

extern player::engine player_engine;


#endif
