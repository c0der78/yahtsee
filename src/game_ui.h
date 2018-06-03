//
// Created by Ryan Jennings on 2018-02-26.
//

#ifndef YAHTSEE_UI_H
#define YAHTSEE_UI_H

#include <functional>
#include <unordered_map>
#include <coda/dice/yaht/scoresheet.h>

#include "renderable.h"
#include "updatable.h"

namespace yahtsee {

    class Player;

    namespace impl {
        template<typename T>
        class Option {
        public:
            using Callback = std::function<void(const T &, const std::string &)>;

            Option(T *owner, const std::string &description, const Callback &callback = nullptr) : owner_(owner),
                    description_(description), callback_(callback) {}

            std::string description() const { return description_; }

            Callback callback() const { return callback_; }

        private:
            T *owner_;
            std::string description_;
            Callback callback_;
        };
    }

    class Dialog {
    public:
        using Option = impl::Option<Dialog>;
        Dialog(const std::string &message);
        Dialog();
        Dialog &add_option(const std::string &key, const std::shared_ptr<Option> &option);
        Dialog &add_option(const std::string &key, const std::string &description, const Option::Callback &callback = nullptr);
    private:
        std::unordered_map<std::string, std::shared_ptr<Option>> options_;
        std::string message_;
    };

    class Menu : public Renderable, public Updatable {
    public:
        using Option = impl::Option<Menu>;
        Menu();
        virtual Menu &add_option(const std::string &key, const std::shared_ptr<Option> &option);
        Menu &add_option(const std::string &key, const std::string &description, const Option::Callback &callback = nullptr);
    protected:
        std::unordered_map<std::string, std::shared_ptr<Option>> options_;
    };

    class GameUi : public Renderable, public Updatable {

    public:
        /*! a state handler */
        typedef void (GameUi::*Handler)();

        virtual void set_needs_refresh() = 0;

        /*! alerts */

        virtual std::shared_ptr<Menu> menu() = 0;

        virtual void show(const Dialog &dialog) = 0;

        virtual void flash(const Dialog &dialog, const std::function<void()> &onFinish = nullptr) = 0;

        virtual void modal(const Dialog &dialog) = 0;

        /** these functions candidates for removal **/

        /*! get input from UI */

        virtual void already_scored() = 0;

        virtual void dice(const std::shared_ptr<Player> &player, int x, int y) = 0;

        virtual void help() = 0;

        virtual void ask_name() = 0;

        virtual void dice_roll() = 0;

        virtual void confirm_quit() = 0;

        virtual void ask_number_of_players() = 0;

        virtual void multiplayer_menu() = 0;

        virtual void multiplayer_join() = 0;

        virtual void multiplayer_join_game() = 0;

        virtual void player_scores() = 0;

        virtual void hosting_game() = 0;

        virtual coda::yaht::scoresheet::value_type upper_scores(int color, const coda::yaht::scoresheet &score, int x, int y) = 0;

        virtual void lower_scores(int color, const coda::yaht::scoresheet &score,
                                  coda::yaht::scoresheet::value_type lower_score_total, int x, int y) = 0;

        virtual void client_waiting_to_start() = 0;

        virtual void waiting_for_connections() = 0;

        virtual void waiting_for_players() = 0;

        virtual void joining_game() = 0;
        /** end removal candidates **/
    };

    class StateManager;

    namespace factory {
        extern std::shared_ptr<GameUi> new_curses_ui(StateManager *state);
        extern std::shared_ptr<GameUi> new_imgui_ui(StateManager *state);
    }

}

#endif //YAHTSEE_UI_H
