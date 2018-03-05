//
// Created by Ryan Jennings on 2018-02-26.
//

#ifndef YAHTSEE_UI_H
#define YAHTSEE_UI_H

#include <functional>

#include "alert_box.h"
#include <rj/dice/yaht/scoresheet.h>

namespace yahtsee {

    using namespace ui;

    class Player;
    class GameLogic;

    class GameUi {
    public:

        /*! a state handler */
        typedef void (GameUi::*Handler)();

        GameUi(GameLogic *logic);

        void alert(const AlertInput &input);

        void flash_alert(const AlertInput &input, const std::function<void()> &pop = nullptr);

        void modal_alert(const AlertInput &input);

        void pop_alert();

        void already_scored();

        void dice(std::shared_ptr<Player> player, int x, int y);

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

        void refresh();
    private:
        GameLogic *game_;
    };

}

#endif //YAHTSEE_UI_H
