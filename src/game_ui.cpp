
#include "game_ui.h"
#include "game.h"
#include "player.h"
#include <ncurses.h>
#include <coda/log/log.h>
#include <menu.h>

using namespace std;
using namespace coda;

namespace yahtsee
{
    namespace factory {
        std::shared_ptr<GameUi> new_imgui_ui(StateManager *state) {
            return nullptr;
        }
    }

    Dialog::Dialog(const std::string &message) : message_(message) {}

    Dialog::Dialog() {}

    Dialog &Dialog::add_option(const std::string &key, const std::shared_ptr<Option> &option) {
        options_[key] = option;
        return *this;
    }

    Dialog &Dialog::add_option(const std::string &key, const std::string &description, const Option::Callback &callback) {
        return add_option(key, std::make_shared<Option>(this, description, callback));
    }
    Menu::Menu() {}

    Menu &Menu::add_option(const std::string &key, const std::shared_ptr<Option> &option) {
        options_[key] = option;
        return *this;
    }

    Menu &Menu::add_option(const std::string &key, const std::string &description, const Option::Callback &callback) {
        return add_option(key, std::make_shared<Option>(this, description, callback));
    }
}