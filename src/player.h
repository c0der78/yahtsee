#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <rj/dice/yaht/player.h>
#include <nlohmann/json.hpp>
#include <queue>

class connection;

using nlohmann::json;

/*!
 * An entity that is playing the game
 * Has an id, name, score and dice.
 */
class player : public rj::yaht::player
{
   public:
    typedef nlohmann::json packet_format;

    player(const string &name);

    player(connection *conn, const string &id, const string &name);

    player(connection *conn, const json &json);

    player(const player &other);

    player(player &&other);

    virtual ~player();

    player &operator=(const player &other);
    player &operator=(player &&other);

    string id() const;

    string name() const;

    void from_packet(const packet_format &packet);
    packet_format to_packet() const;

    connection *get_connection() const;

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
