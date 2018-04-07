//
// Created by Ryan Jennings on 2018-02-26.
//

#include "game_ui.h"
#include "game.h"
#include "player.h"
#include <rj/log/log.h>
#include <curses.h>
#include <menu.h>

using namespace std;
using namespace rj;

namespace yahtsee
{

    class CursesMenu : public Menu {
    public:
        CursesMenu(InputManager *input) : input_(input) {}

        ~CursesMenu() {
            if (menu_ != nullptr) {
                unpost_menu(menu_);
            }
            for(int i = 0; i < size_; ++i) {
                free_item(items_[i]);
            }
            if (menu_ != nullptr) {
                free_menu(menu_);
            }
            if (window_ != nullptr) {
                delwin(window_);
            }
        }

        void render() {
            if (menu_ == nullptr) {
                init();
                menu_driver(menu_, REQ_FIRST_ITEM);
            }
            post_menu(menu_);
            wrefresh(window_);
        }

        void update() {
            switch(input_->get())
            {	case KEY_DOWN:
                    menu_driver(menu_, REQ_DOWN_ITEM);
                    break;
                case KEY_UP:
                    menu_driver(menu_, REQ_UP_ITEM);
                    break;
                case 10: /* Enter */
                    auto option = item_name(current_item(menu_));
                    auto callback = options_[option].second;
                    pos_menu_cursor(menu_);
                    if (callback != nullptr) {
                        callback(*this, option);
                    }
                    break;
            }
        }
    private:
        void init() {
            size_ = options_.size();
            items_ = (ITEM **) calloc(size_ + 1, sizeof(ITEM *));
            int i = 0;
            for (auto &entry : options_) {
                items_[i++] = new_item(entry.first.c_str(), entry.second.first.c_str());
            }
            items_[size_] = nullptr;
            menu_ = new_menu(items_);

            /* Set fore ground and back ground of the menu */
            set_menu_fore(menu_, COLOR_PAIR(1) | A_REVERSE);
            set_menu_back(menu_, COLOR_PAIR(2));
            set_menu_grey(menu_, COLOR_PAIR(3));

            window_ = newwin(10, 40, 4, 4);
            keypad(window_, TRUE);
            set_menu_win(menu_, window_);
            set_menu_sub(menu_, derwin(window_, 6, 38, 3, 1));

            set_menu_mark(menu_, " * ");

            box(window_, 0, 0);
            print_in_middle(window_, 1, 0, 40, "Menu", COLOR_PAIR(1));
            mvwaddch(window_, 2, 0, ACS_LTEE);
            mvwhline(window_, 2, 1, ACS_HLINE, 38);
            mvwaddch(window_, 2, 39, ACS_RTEE);
//            mvprintw(LINES - 2, 0, "ESC to exit");

        }

        void print_in_middle(WINDOW *win, int starty, int startx, int width, const char *string, chtype color)
        {	int length, x, y;
            float temp;

            if(win == NULL)
                win = stdscr;
            getyx(win, y, x);
            if(startx != 0)
                x = startx;
            if(starty != 0)
                y = starty;
            if(width == 0)
                width = 80;

            length = strlen(string);
            temp = (width - length)/ 2;
            x = startx + (int)temp;
            wattron(win, color);
            mvwprintw(win, y, x, "%s", string);
            wattroff(win, color);
            refresh();
        }

        InputManager *input_;
        MENU *menu_;
        ITEM **items_;
        WINDOW *window_;
        int size_;
    };

    Dialog::Dialog(const std::string &message) : message_(message) {}

    Dialog::Dialog() {}

    Dialog &Dialog::add_option(const std::string &name, const Callback &value) {
        options_[name] = value;
        return *this;
    }

    Menu::Menu() {}

    Menu &Menu::add_option(const std::string &name, const std::string &description, const Callback &value) {
        options_[name] = std::pair<std::string, Callback>(description, value);
        return *this;
    }

    CursesUi::CursesUi(StateManager *state) : state_(state) {
        initscr();
        raw();
        start_color();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);

        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    }

    CursesUi::~CursesUi() {
        endwin();
    }

    std::shared_ptr<Menu> CursesUi::menu() {
        return std::make_shared<CursesMenu>(state_->input());
    }

    void CursesUi::show(const Dialog &dialog) {

    }

    void CursesUi::flash(const Dialog &dialog, const std::function<void()> &onFinish) {}

    void CursesUi::modal(const Dialog &dialog) {}

    void CursesUi::joining_game() {

    }
    void CursesUi::set_needs_refresh() {
        refresh();
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
        std::string message;

        char buf[BUFSIZ + 1] = {0};
        snprintf(buf, BUFSIZ, "Waiting for connections on port %d...", state_->online()->port());

        message += buf;
        message += "\n";

        if (!state_->online()->has_registry()) {
            message += "\n";
            message += "Visit ";
            message += state_->online()->registry_url();
            message += " to learn how to configure yahtsee for automatic online registration.";
        }

        show(message);
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

        string message;

        int count = 1;

        for (const auto &p : state_->players()->all()) {
            snprintf(buf, BUFSIZ, "%2d: %s", count++, p->name().c_str());
            message += buf;
        }

        message += "\n\n";

        message += "Waiting for host to start game...";

        log::trace("Waiting for host to start game...");

        show(message);
    }


    void CursesUi::already_scored()
    {
        flash(Dialog("You've already scored that."));
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
        refresh();
    }

    void CursesUi::update() {
        switch(state_->input()->get()) {
            case '\033':
                state_->logic()->enlighten();
                break;
        }
    }
}