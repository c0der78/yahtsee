#ifndef YAHTSEE_PLAYER_H
#define YAHTSEE_PLAYER_H

#include <rj/dice/yaht/player.h>
#include <nlohmann/json.hpp>
#include <queue>

using nlohmann::json;

namespace yahtsee {

    class connection;

    /*!
     * An entity that is playing the game
     * Has an id, name, score and dice.
     */
    class Player : public rj::yaht::player {
    public:
        typedef nlohmann::json Packet;
        typedef nlohmann::json Config;

        Player(const std::string &name);

        Player(connection *conn, const std::string &id, const std::string &name);

        Player(connection *conn, const Config &config);

        Player(const Player &other);

        Player(Player &&other);

        virtual ~Player();

        Player &operator=(const Player &other);

        Player &operator=(Player &&other);

        std::string id() const;

        std::string name() const;

        void from_packet(const Packet &packet);

        Packet to_packet() const;

        Connection *get_connection() const;

        bool operator==(const Player &other) const;

        class Engine : public rj::die::engine {
        public:
            rj::die::value_type generate(rj::die::value_type from, rj::die::value_type to);

            void reset();

            void set_next_roll(const std::queue<rj::die::value_type> &values);

        private:
            std::queue<rj::die::value_type> nextRoll_;

            friend class Player;
        };

    private:
        Connection *connection_;
        std::string id_;
        std::string name_;
    };

    extern Player::Engine playerEngine;


}
#endif
