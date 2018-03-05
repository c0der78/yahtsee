//
// Created by Ryan Jennings on 2018-02-26.
//

#ifndef YAHTSEE_GAME_H
#define YAHTSEE_GAME_H

#include <memory>
#include <vector>

#include "game_ui.h"
#include "game_action.h"
#include "matchmaker.h"
#include "player.h"

namespace yahtsee {

    class OnlineInfo : public Matchmaker {
    public:
        using Matchmaker::Matchmaker;

        // registry service
        bool has_registry() const;
        OnlineInfo& set_registry_url(const std::string &url);
        std::string registry_url() const;

        bool is_matchmaking() const;
        bool is_hosting() const;
        void reset();
    private:
        int flags_;
        std::string registryUrl_;

        static const int HOSTING = (1 << 0);
        static const int JOINING = (1 << 1);
        friend class GameLogic;
    };

    class GameLogic {
    public:

        typedef nlohmann::json Config;
        typedef std::vector<Player::Ref> PlayerList;
        typedef PlayerList::const_iterator PlayerTurn;

        GameLogic();

        // players
        GameLogic& add_player(const Player::Ref &player);
        Player::Ref player() const;
        Player::Ref turn() const;
        GameLogic &set_turn(const Player::Ref &player);
        const PlayerList &&players() const;
        Player::Ref find_player_by_id(const std::string &id) const;
        Player::Ref find_player(const std::function<bool(const Player::Ref &p)> &delegate) const;
        bool is_single_player() const;
        bool remove_player(const Player::Ref &player);
        Player::Ref next_turn();

        const std::unique_ptr<GameAction> &&actions() const;

        const std::unique_ptr<GameUi> &&ui() const;

        const std::unique_ptr<OnlineInfo> &&online() const;

        void reset();

        const Config &settings() const;
    private:
        PlayerList players_;
        PlayerTurn turn_;
        std::unique_ptr<GameUi> ui_;
        std::unique_ptr<GameAction> actions_;
        std::unique_ptr<OnlineInfo> online_;
        Config settings_;
    };


    class Game {
    public:
        Game(int seed);

        Game &load();

        Game &begin();

        bool on() const;

        Game &update();

        Game &end();

        std::unique_ptr<GameLogic> &logic();

    private:
        std::unique_ptr<GameLogic> logic_;
    };


}

#endif //YAHTSEE_GAME_H
