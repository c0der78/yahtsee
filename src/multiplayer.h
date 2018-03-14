//
// Created by Ryan Jennings on 2018-03-05.
//

#ifndef YAHTSEE_MULTIPLAYER_H
#define YAHTSEE_MULTIPLAYER_H

#include "matchmaker.h"

namespace yahtsee {

    class Multiplayer : public Matchmaker {
    public:
        using Matchmaker::Matchmaker;

        // registry service
        bool has_registry() const;
        Multiplayer& set_registry_url(const std::string &url);
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

}

#endif //YAHTSEE_MULTIPLAYER_H
