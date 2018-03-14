//
// Created by Ryan Jennings on 2018-02-26.
//

#ifndef YAHTSEE_UI_H
#define YAHTSEE_UI_H

#include <functional>
#include <rj/dice/yaht/scoresheet.h>

#include "alert_box.h"
#include "renderable.h"
#include "updatable.h"


namespace yahtsee {

    using namespace ui;

    class Player;

    class GameUi : public Renderable, public Updatable {

    public:
        /*! a state handler */
        typedef void (GameUi::*Handler)();

        virtual void alert(const AlertInput &input) = 0;

        virtual void flash_alert(const AlertInput &input, const std::function<void()> &pop = nullptr) = 0;

        virtual void modal_alert(const AlertInput &input) = 0;

        virtual void pop_alert() = 0;

        virtual void already_scored() = 0;

        virtual void dice(const std::shared_ptr<Player> &player, int x, int y) = 0;

        virtual void help() = 0;

        virtual void menu() = 0;

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

        virtual void set_needs_refresh() = 0;

    };


    class StateManager;

    class CursesUi : public GameUi {
    public:
        CursesUi(StateManager *state);

        void alert(const AlertInput &input);

        void flash_alert(const AlertInput &input, const std::function<void()> &pop = nullptr);

        void modal_alert(const AlertInput &input);

        void pop_alert();

        void already_scored();

        void dice(const std::shared_ptr<Player> &player, int x, int y);

        void help();

        void menu();

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

        void set_needs_refresh();

        void update();

        void render();
    private:
        StateManager *state_;
    };

    class ImGUi : public GameUi {};
}

#endif //YAHTSEE_UI_H
