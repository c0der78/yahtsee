//
// Created by Ryan Jennings on 2018-03-13.
//

#ifndef YAHTSEE_PLAYER_MANAGER_H
#define YAHTSEE_PLAYER_MANAGER_H

#include <memory>
#include <vector>
#include <functional>

namespace yahtsee {

    class Player;

    class PlayerManager {
    public:
        using PlayerList = std::vector<std::shared_ptr<Player>>;
        using PlayerTurn = PlayerList::const_iterator;

        // players
        const PlayerList &all() const;
        std::shared_ptr<Player> self() const;

        // mutators
        PlayerManager& add(const std::shared_ptr<Player> &player);
        bool remove(const std::shared_ptr<Player> &player);

        // queries
        std::shared_ptr<Player> find_by_id(const std::string &id) const;
        std::shared_ptr<Player> find(const std::function<bool(const std::shared_ptr<Player> &p)> &delegate) const;

        // turns
        std::shared_ptr<Player> next_turn();
        std::shared_ptr<Player> turn() const;
        PlayerManager &set_turn(const std::shared_ptr<Player> &player);

        bool is_single_player() const;

        void reset();
    private:
        PlayerList players_;
        PlayerTurn turn_;
    };

}


#endif //YAHTSEE_PLAYER_MANAGER_H
