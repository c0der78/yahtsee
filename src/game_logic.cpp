//
// Created by Ryan Jennings on 2018-03-04.
//

#include "game_logic.h"

namespace yahtsee
{
    GameLogic::GameLogic() {
    }

    GameLogic& GameLogic::add_player(const Player::Ref &value) {
        players_.push_back(value);
        return *this;
    }

    Player::Ref GameLogic::player() const {
        if (players_.empty()) {
            return nullptr;
        }
        return players_.front();
    }

    Player::Ref GameLogic::turn() const {
        if (turn_ == players_.end()) {
            return nullptr;
        }
        return *turn_;
    }

    Player::Ref GameLogic::next_turn() {

        if (++turn_ == players_.end()) {
            turn_ = players_.begin();
        }

        return *turn_;
    }

    GameLogic &GameLogic::set_turn(const Player::Ref &player) {
        turn_ = std::find(players_.begin(), players_.end(), player);
        return *this;
    }

    const std::unique_ptr<OnlineInfo> &&GameLogic::online() const {
        return std::move(online_);
    }


    bool OnlineInfo::is_matchmaking() const {
        return flags_ != 0;
    }

    const GameLogic::PlayerList &&GameLogic::players() const {
        return std::move(players_);
    }

    const std::unique_ptr<GameAction> &&GameLogic::actions() const {
        return std::move(actions_);
    }


    const std::unique_ptr<GameUi> &&GameLogic::ui() const {
        return std::move(ui_);
    }

    bool OnlineInfo::is_hosting() const {
        return (flags_ & HOSTING) != 0;
    }

    void OnlineInfo::reset() {
        flags_ = 0;
        stop();
    }

    Player::Ref GameLogic::find_player_by_id(const string &id) const
    {
        return find_player([&id](const Player::Ref &p) {
            return p->id() == id;
        });
    }

    Player::Ref GameLogic::find_player(const std::function<bool(const Player::Ref &p)> &delegate) const {
        for (const auto &player : players_) {
            if (delegate(player)) {
                return player;
            }
        }

        return nullptr;
    }

    void GameLogic::reset() {

        online_->reset();

        players_.clear();
    }

    bool GameLogic::is_single_player() const {
        return players_.size() == 1;
    }

    const GameLogic::Config &GameLogic::settings() const {
        return settings_;
    }

    bool GameLogic::remove_player(const Player::Ref &player) {
        auto it = std::find(players_.begin(), players_.end(), player);

        if (it == players_.end()) {
            return false;
        }

        players_.erase(it);
        return true;
    }

    bool OnlineInfo::has_registry() const {
        return !registryUrl_.empty();
    }

    std::string OnlineInfo::registry_url() const {
        return registryUrl_;
    }

    OnlineInfo& OnlineInfo::set_registry_url(const std::string &value) {
        registryUrl_ = value;
        return *this;
    }

}