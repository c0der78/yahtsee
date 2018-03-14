//
// Created by Ryan Jennings on 2018-03-13.
//

#include "player_manager.h"
#include "player.h"

namespace yahtsee {

    void PlayerManager::reset() {
        players_.clear();
        turn_ = players_.begin();
    }
    PlayerManager& PlayerManager::add(const std::shared_ptr<Player> &value) {
        players_.push_back(value);
        return *this;
    }

    std::shared_ptr<Player> PlayerManager::self() const {
        if (players_.empty()) {
            return nullptr;
        }
        return players_.front();
    }

    std::shared_ptr<Player> PlayerManager::turn() const {
        if (turn_ == players_.end()) {
            return nullptr;
        }
        return *turn_;
    }

    std::shared_ptr<Player> PlayerManager::next_turn() {

        if (++turn_ == players_.end()) {
            turn_ = players_.begin();
        }

        return *turn_;
    }

    PlayerManager &PlayerManager::set_turn(const std::shared_ptr<Player> &player) {
        turn_ = std::find(players_.begin(), players_.end(), player);
        return *this;
    }

    // TODO: use iterator on manager
    const PlayerManager::PlayerList &PlayerManager::all() const {
        return players_;
    }

    std::shared_ptr<Player> PlayerManager::find_by_id(const std::string &id) const
    {
        return find([&id](const std::shared_ptr<Player> &p) {
            return p->id() == id;
        });
    }

    std::shared_ptr<Player> PlayerManager::find(const std::function<bool(const std::shared_ptr<Player> &p)> &delegate) const {
        for (const auto &player : players_) {
            if (delegate(player)) {
                return player;
            }
        }

        return nullptr;
    }

    bool PlayerManager::is_single_player() const {
        return players_.size() == 1;
    }


    bool PlayerManager::remove(const std::shared_ptr<Player> &player) {
        auto it = std::find(players_.begin(), players_.end(), player);

        if (it == players_.end()) {
            return false;
        }

        players_.erase(it);
        return true;
    }
}