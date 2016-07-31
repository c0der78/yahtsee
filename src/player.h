#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <rj/dice/yaht/player.h>
#include <rj/json/object.h>
#include <queue>

class connection;

/*!
 * An entity that is playing the game
 * Has an id, name, score and dice.
 */
class player : public rj::yaht::player
{
   public:
    player(const string &name);

    player(connection *conn, const string &id, const string &name);

    player(connection *conn, const rj::json::object &json);

    player(const player &other);

    player(player &&other);

    virtual ~player();

    player &operator=(const player &other);
    player &operator=(player &&other);

    string id() const;

    string name() const;

    void from_json(const rj::json::object &json);
    rj::json::object to_json() const;

    connection *c0nnection() const;

    bool operator==(const player &other) const;

    class engine : public rj::die::engine
    {
       public:
        rj::die::value_type generate(rj::die::value_type from, rj::die::value_type to);
        void reset();
        void set_next_roll(const queue<rj::die::value_type> &values);

       private:
        queue<rj::die::value_type> nextRoll_;
        friend class player;
    };

   private:
    connection *connection_;
    string id_;
    string name_;
};

extern player::engine player_engine;


#endif
