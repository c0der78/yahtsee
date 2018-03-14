//
// Created by Ryan Jennings on 2018-03-05.
//

#include "multiplayer.h"


namespace yahtsee
{
    bool Multiplayer::is_matchmaking() const {
        return flags_ != 0;
    }


    bool Multiplayer::is_hosting() const {
        return (flags_ & HOSTING) != 0;
    }

    void Multiplayer::reset() {
        flags_ = 0;
        stop();
    }

    bool Multiplayer::has_registry() const {
        return !registryUrl_.empty();
    }

    std::string Multiplayer::registry_url() const {
        return registryUrl_;
    }

    Multiplayer& Multiplayer::set_registry_url(const std::string &value) {
        registryUrl_ = value;
        return *this;
    }

}