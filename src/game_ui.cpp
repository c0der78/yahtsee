//
// Created by Ryan Jennings on 2018-02-26.
//

#include "game_ui.h"
#include "game.h"
#include "player.h"
#include <rj/log/log.h>

using namespace std;
using namespace rj;

namespace yahtsee
{

    using namespace ui;

    CursesUi::CursesUi(StateManager *state) : state_(state) {}

    void CursesUi::alert(const AlertInput &input) {}

    void CursesUi::flash_alert(const AlertInput &input, const std::function<void()> &pop) {}

    void CursesUi::modal_alert(const AlertInput &input) {}

    void CursesUi::pop_alert() {
    }

    void CursesUi::joining_game() {

    }
    void CursesUi::set_needs_refresh() {
        // TODO: refresh display
    }
    void CursesUi::menu()
    {
        // TODO: display or create menu
    }

    void CursesUi::ask_name()
    {
        // TODO: input players name
    }

    void CursesUi::ask_number_of_players()
    {
        // TODO: input number of players
    }

    void CursesUi::waiting_for_connections() {
        std::vector<std::string> messages;

        char buf[BUFSIZ + 1] = {0};
        snprintf(buf, BUFSIZ, "Waiting for connections on port %d...", state_->online()->port());

        messages.push_back(buf);

        if (!state_->online()->has_registry()) {
            messages.push_back("");
            messages.push_back("Visit " + state_->online()->registry_url() + " to learn how to");
            messages.push_back("configure yahtsee for automatic online registration.");
        }

        alert(messages);
    }

    void CursesUi::multiplayer_menu()
    {
        // TODO: display options to host or join a game
    }

    void CursesUi::multiplayer_join()
    {
        // TODO: display options to join online or lan
    }

    void CursesUi::multiplayer_join_game()
    {
        // TODO: input lan address:port

    }

    void CursesUi::hosting_game()
    {
        // TODO: display hosting/waiting message
    }

    void CursesUi::client_waiting_to_start()
    {
        char buf[BUFSIZ + 1] = {0};

        vector<string> message;

        int count = 1;

        for (const auto &p : state_->players()->all()) {
            snprintf(buf, BUFSIZ, "%2d: %s", count++, p->name().c_str());
            message.push_back(buf);
        }

        message.push_back(" ");

        message.push_back("Waiting for host to start game...");

        log::trace("Waiting for host to start game...");

        alert(message);
    }


    void CursesUi::already_scored()
    {
        flash_alert("You've already scored that.");
    }

    void CursesUi::dice(const std::shared_ptr<Player>& player, int x, int y)
    {
        // TODO: display dice rolling ui
        //     : allow to roll 3 times
        //     : options to keep specific die
    }

    void CursesUi::dice_roll()
    {
        // TODO: roll dice and display
    }


    void CursesUi::confirm_quit()
    {
        // TODO: confirm quit
    }


    void CursesUi::help()
    {
        // TODO: display help text
    }


    void CursesUi::player_scores()
    {
        // TODO: display player score on card
    }

    yaht::scoresheet::value_type CursesUi::upper_scores(int color, const yaht::scoresheet &score, int x, int y)
    {
        // TODO: display upper score on card and return total
        return 0;
    }

    void CursesUi::lower_scores(int color, const yaht::scoresheet &score, yaht::scoresheet::value_type lower_score_total, int x, int y)
    {
        // TODO display lower score on card

    }


    void CursesUi::waiting_for_players() {
        // TODO: display model with current players and option to start game
    }

    void CursesUi::render() {

    }

    void CursesUi::update() {

    }
}