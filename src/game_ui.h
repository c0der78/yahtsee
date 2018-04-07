//
// Created by Ryan Jennings on 2018-02-26.
//

#ifndef YAHTSEE_UI_H
#define YAHTSEE_UI_H

#include <functional>
#include <unordered_map>
#include <rj/dice/yaht/scoresheet.h>

#include "renderable.h"
#include "updatable.h"


namespace yahtsee {

    class Player;

    class Dialog {
    public:
        using Callback = std::function<int(const Dialog &, const std::string &)>;
        Dialog(const std::string &message);
        Dialog();
        Dialog &add_option(const std::string &name, const Callback &value = nullptr);
    private:
        std::unordered_map<std::string, Callback> options_;
        std::string message_;
    };

    class Menu : public Renderable, public Updatable {
    public:
        using Callback = std::function<void(const Menu &, const std::string &)>;
        Menu();
        virtual Menu &add_option(const std::string &name, const std::string &description, const Callback &value = nullptr);
    protected:
        std::unordered_map<std::string, std::pair<std::string, Callback>> options_;
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

        virtual rj::yaht::scoresheet::value_type upper_scores(int color, const rj::yaht::scoresheet &score, int x, int y) = 0;

        virtual void lower_scores(int color, const rj::yaht::scoresheet &score,
                                  rj::yaht::scoresheet::value_type lower_score_total, int x, int y) = 0;

        virtual void client_waiting_to_start() = 0;

        virtual void waiting_for_connections() = 0;

        virtual void waiting_for_players() = 0;

        virtual void joining_game() = 0;
        /** end removal candidates **/

    };

    class StateManager;

    class CursesUi : public GameUi {
    public:
        CursesUi(StateManager *state);

        ~CursesUi();

        std::shared_ptr<Menu> menu();

        void show(const Dialog &dialog);

        void flash(const Dialog &dialog, const std::function<void()> &onFinish = nullptr);

        void modal(const Dialog &dialog);

        void set_needs_refresh();

        void update();

        void render();

        /** removal candidates **/

        void already_scored();

        void dice(const std::shared_ptr<Player> &player, int x, int y);

        void help();

        void ask_name();

        void dice_roll();

        void confirm_quit();

        void ask_number_of_players();

        void multiplayer_menu();

        void multiplayer_join();

        void multiplayer_join_game();

        void player_scores();

        void hosting_game();

        rj::yaht::scoresheet::value_type upper_scores(int color, const rj::yaht::scoresheet &score, int x, int y);

        void lower_scores(int color, const rj::yaht::scoresheet &score,
                                  rj::yaht::scoresheet::value_type lower_score_total, int x, int y);

        void client_waiting_to_start();

        void waiting_for_connections();

        void waiting_for_players();

        void joining_game();
        /** end removal candidates **/

    private:
        StateManager *state_;
    };

    class ImGUi : public GameUi {};
}

#endif //YAHTSEE_UI_H
