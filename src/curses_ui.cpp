
#include "game_ui.h"
#include "game.h"
#include "state_manager.h"
#include "player.h"
#include <coda/log/log.h>
#include <ncurses.h>
#include <menu.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <algorithm>

using namespace coda;

namespace yahtsee
{
    namespace curses {

        class Ui : public GameUi {
        public:
            Ui(StateManager *state);

            ~Ui();

            std::shared_ptr<yahtsee::Menu> menu();

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

            coda::yaht::scoresheet::value_type upper_scores(int color, const coda::yaht::scoresheet &score, int x, int y);

            void lower_scores(int color, const coda::yaht::scoresheet &score,
                              coda::yaht::scoresheet::value_type lower_score_total, int x, int y);

            void client_waiting_to_start();

            void waiting_for_connections();

            void waiting_for_players();

            void joining_game();
            /** end removal candidates **/

        private:
            StateManager *state_;
            WINDOW *window_;
        };


        class Menu : public yahtsee::Menu {
        public:
            Menu(InputManager *input) : input_(input) {}

            ~Menu() {
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
                {
                    case KEY_DOWN:
                        menu_driver(menu_, REQ_DOWN_ITEM);
                        break;
                    case KEY_UP:
                        menu_driver(menu_, REQ_UP_ITEM);
                        break;
                    case KEY_LEFT:
                        menu_driver(menu_, REQ_LEFT_ITEM);
                        break;
                    case KEY_RIGHT:
                        menu_driver(menu_, REQ_RIGHT_ITEM);
                        break;
                    case KEY_NPAGE:
                        menu_driver(menu_, REQ_SCR_DPAGE);
                        break;
                    case KEY_PPAGE:
                        menu_driver(menu_, REQ_SCR_UPAGE);
                        break;
                    case ' ':
                        menu_driver(menu_, REQ_TOGGLE_ITEM);
                        break;
                    case 10: /* Enter */
                        pos_menu_cursor(menu_);
                        auto key = item_name(current_item(menu_));
                        auto it = options_.find(key);
                        if (it != options_.end()) {
                            auto callback = it->second->callback();
                            if (callback != nullptr) {
                                callback(*this, key);
                            }
                        }
                        break;
                }
            }
        private:
            void init() {
                size_ = options_.size();
                items_ = (ITEM **) calloc(size_ + 1, sizeof(ITEM *));
                int i = 0;

                const int height = size_ + 4;
                int width = 0;

                for (auto &entry : options_) {
                    auto descr = entry.second->description();
                    auto item = new_item(entry.first.c_str(), descr.c_str());
                    
                     if (item == nullptr) {
                        log::error("unable to create item");
                        continue;
                     }
                     items_[i++] = item;

                    if (descr.size() > width) {
                        width = descr.size();
                    }
                }
                width += 8; // for border, space and mark

                items_[size_] = nullptr;
                menu_ = new_menu(items_);

                /* Set fore ground and back ground of the menu */
                set_menu_fore(menu_, COLOR_PAIR(1) | A_REVERSE);
                set_menu_back(menu_, COLOR_PAIR(2));
                set_menu_grey(menu_, COLOR_PAIR(3));

                window_ = newwin(height, width, (LINES/2)-(height/2), (COLS/2)-(width/2));
                keypad(window_, TRUE);
                set_menu_win(menu_, window_);
                set_menu_sub(menu_, derwin(window_, height-4, width-2, 3, 1));
                set_menu_mark(menu_, " * ");

                box(window_, 0, 0);
                print_in_middle(window_, 1, 0, width, "Menu", COLOR_PAIR(1));
                mvwaddch(window_, 2, 0, ACS_LTEE);
                mvwhline(window_, 2, 1, ACS_HLINE, width-2);
                mvwaddch(window_, 2, width-1, ACS_RTEE);
                refresh();
            }

            void print_in_middle(WINDOW *win, int starty, int startx, int width, const char *string, chtype color)
            {
                int length, x, y;
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
            }

            InputManager *input_;
            MENU *menu_;
            ITEM **items_;
            WINDOW *window_;
            int size_;
        };

        void handle_winch(int sig){

            struct winsize w;
            ioctl(0, TIOCGWINSZ, &w);
            int cols = w.ws_col;
            int lines = w.ws_row;

            wresize(stdscr, lines, cols);

            refresh();
        }

        Ui::Ui(StateManager *state) : state_(state) {
            window_ = initscr();
            raw();
            cbreak();
            noecho();
            keypad(window_, TRUE);
            curs_set(0);

            if (has_colors()) {
                start_color();
                init_pair(1, COLOR_RED, COLOR_BLACK);
                init_pair(2, COLOR_GREEN, COLOR_BLACK);
                init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
            }

            wborder(window_, L'|', L'|', L'-', L'-', L'⎾', L'⏋', L'⎿', L'⏌');
    
            struct sigaction sa;
            memset(&sa, 0, sizeof(struct sigaction));
            sa.sa_handler = handle_winch;
            sigaction(SIGWINCH, &sa, NULL);
        }

        Ui::~Ui() {
            delwin(window_);
            endwin();
        }

        std::shared_ptr<yahtsee::Menu> Ui::menu() {
            return std::make_shared<Menu>(state_->input());
        }

        void Ui::show(const Dialog &dialog) {
        }

        void Ui::flash(const Dialog &dialog, const std::function<void()> &onFinish) {}

        void Ui::modal(const Dialog &dialog) {}

        void Ui::joining_game() {

        }
        void Ui::set_needs_refresh() {
        }

        void Ui::ask_name()
        {
            // TODO: input players name
        }

        void Ui::ask_number_of_players()
        {
            // TODO: input number of players
        }

        void Ui::waiting_for_connections() {
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

        void Ui::multiplayer_menu()
        {
            // TODO: display options to host or join a game
        }

        void Ui::multiplayer_join()
        {
            // TODO: display options to join online or lan
        }

        void Ui::multiplayer_join_game()
        {
            // TODO: input lan address:port
        }

        void Ui::hosting_game()
        {
            // TODO: display hosting/waiting message
        }

        void Ui::client_waiting_to_start()
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


        void Ui::already_scored()
        {
            flash(Dialog("You've already scored that."));
        }

        void Ui::dice(const std::shared_ptr<Player>& player, int x, int y)
        {
            // TODO: display dice rolling ui
            //     : allow to roll 3 times
            //     : options to keep specific die
        }

        void Ui::dice_roll()
        {
            // TODO: roll dice and display
        }


        void Ui::confirm_quit()
        {
            // TODO: confirm quit
        }


        void Ui::help()
        {
            // TODO: display help text
        }


        void Ui::player_scores()
        {
            // TODO: display player score on card
        }

        yaht::scoresheet::value_type Ui::upper_scores(int color, const yaht::scoresheet &score, int x, int y)
        {
            // TODO: display upper score on card and return total
            return 0;
        }

        void Ui::lower_scores(int color, const yaht::scoresheet &score, yaht::scoresheet::value_type lower_score_total, int x, int y)
        {
            // TODO display lower score on card
        }

        void Ui::waiting_for_players() {
            // TODO: display model with current players and option to start game
        }

        void Ui::render() {
            wrefresh(window_);
            refresh();
        }

        void Ui::update() {
            switch(state_->input()->get()) {
                case '\033':
                    state_->logic()->stop_thinking();
                    break;
            }
        }
    }

    namespace factory {
        std::shared_ptr<GameUi> new_curses_ui(StateManager *state) {
            return std::make_shared<curses::Ui>(state);
        }
    }
}