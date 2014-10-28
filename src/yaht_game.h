
#include "caca_game.h"

#include <arg3dice/yaht/engine.h>

using namespace arg3::yaht;

typedef enum
{
    ASK_NAME,
    PLAYING,
    ROLLING_DICE,
    DISPLAY_MENU,
    QUIT,
    QUIT_CONFIRM
} game_state;

typedef enum
{
    HORIZONTAL,
    VERTICAL,
    MINIMAL
} display_mode;

static const char *HELP = "Type '?' to show command options.  Use the arrow keys to cycle views modes.";

class yaht_game : public caca_game
{
public:
    yaht_game() : upperbuf_(NULL), lowerbuf_(NULL), menubuf_(NULL), headerbuf_(NULL), upperbuf_size_(0), lowerbuf_size_(0), menubuf_size_(0), headerbuf_size_(0), display_mode_(MINIMAL)
    {}

    void reset()
    {
        caca_game::reset();


        if (upperbuf_ != NULL)
        {
            free(upperbuf_);
            upperbuf_ = NULL;
            upperbuf_size_ = 0;
        }

        if (lowerbuf_ != NULL)
        {
            free(lowerbuf_);
            lowerbuf_ = NULL;
            lowerbuf_size_ = 0;
        }

        if (menubuf_ != NULL)
        {
            free(menubuf_);
            menubuf_ = NULL;
            menubuf_size_ = 0;
        }
    }
    game_state state() const
    {
        return state_;
    }

    void recover_state()
    {
        state_ = last_state_;

        new_frame();
    }

    void set_state(game_state value)
    {
        last_state_ = state_;

        state_ = value;

        new_frame();
    }

    void on_start()
    {
        set_state(ASK_NAME);
    }

    void prompt()
    {
        switch (state_)
        {
        case ASK_NAME:
            display_alert([&](const alert_box & a)
            {
                put(a.center_x() - 10, a.center_y() - 1, "What is your name? ");
                set_cursor(a.center_x() - 10, a.center_y());
            });
            break;
        case ROLLING_DICE:
            display_alert([&](const alert_box & box)
            {
                display_dice(engine::instance()->current_player(), box.x(), box.y());
            });

        case DISPLAY_MENU:
        default:
            break;
        }

    }

    bool alive() const
    {
        return state_ != QUIT;
    }

    void refresh_display(bool reset)
    {
        player *player = engine::instance()->current_player();

        if (player != NULL && state_ == PLAYING)
        {
            if (reset)
                player->reset();

            int x = 46;

            put(50, 2, player->name().c_str());

            switch (display_mode_)
            {
            case MINIMAL:
                if (minimalLower_)
                {
                    display_lower_scores(player->score(), player->calculate_total_upper_score(), x, 9);
                    put(0, 32, HELP);
                }
                else
                {
                    display_upper_scores(player->score(), x , 9 );
                    put(0, 27, HELP);
                }
                break;
            case VERTICAL:
            {
                scoresheet::value_type lower_score_total = lower_score_total = display_upper_scores(player->score(), x , 9 );
                display_lower_scores(player->score(), lower_score_total, 46, 28);
                put(0, 51, HELP);
                break;
            }
            case HORIZONTAL:
            {
                scoresheet::value_type lower_score_total = display_upper_scores(player->score(), x , 9 );
                display_lower_scores(player->score(), lower_score_total, 122, 2);
                put(76, 25, HELP);
                break;
            }
            }

        }

    }


    void display_already_scored()
    {
        display_alert([&](const alert_box & a)
        {
            a.center("You've already scored that.");
        });
        add_event(2000, [&]()
        {
            pop_alert();

            action_display_dice();
        });
    }

    void display_dice(player *player, int x, int y)
    {
        char buf[BUFSIZ + 1] = {0};
        snprintf(buf, BUFSIZ, "Roll %d of 3. (Press '#' to keep):", player->roll_count());

        x += 13;
        y += 4;

        int xs = x;

        put(x, y, buf);

        x += 2;
        y += 2;

        put(x, y++, "#  1 │ 2 │ 3 │ 4 │ 5");
        put(x, y++, "  ───┴───┴───┴───┴───");
        put(x++, y, "  ");
        for (size_t i = 0; i < player->die_count(); i++)
        {
            put(++x, y, player->is_kept(i) ? '*' : ' ');

            put(++x, y, to_string(player->d1e(i).value()).c_str());

            x += 2;
        }
        y += 2;

        put(xs, y, "Press '?' for help on how to score.");
    }

    void action_ask_name(int ch)
    {
        if (ch == CACA_KEY_RETURN)
        {
            string name = get_buffer();

            engine::instance()->add_player(name);

            set_state(PLAYING);

            pop_alert();

            clear_buffer();

            clear();
        }
        else
        {
            add_to_buffer(ch);

            int x = get_cursor_x();

            int y = get_cursor_y();

            put(x + 1, y, ch);

            set_cursor(x + 1, y);
        }

        refresh();
    }

    void action_display_dice()
    {
        display_alert([&](const alert_box & box)
        {
            display_dice(engine::instance()->current_player(), box.x(), box.y());

        });
    }

    void action_roll_dice()
    {
        player *player = engine::instance()->current_player();

        if (player->roll_count() < 3)
        {
            player->roll();

            new_frame();

            action_display_dice();
        }
        else
        {
            display_alert([&](const alert_box & box)
            {
                box.center("You must choose a score after three rolls.");
            });
            add_event(2000, [&]()
            {
                pop_alert();

                action_display_dice();
            });
        }
    }

    void action_select_die(player *player, int d)
    {
        if (player->is_kept(d))
            player->keep_die(d, false);
        else
            player->keep_die(d, true);

        auto box = displayed_alert();

        box.display();
    }


    void action_lower_score(player *player, scoresheet::type type)
    {
        if (!player->score().lower_score(type))
        {
            player->score().lower_score(type, player->calculate_lower_score(type));
            set_state(PLAYING);
            refresh(true);
        }
        else
        {
            display_already_scored();
        }
    }

    void action_score(player *player, int n)
    {
        if (!player->score().upper_score(n))
        {
            player->score().upper_score(n, player->calculate_upper_score(n));

            set_state(PLAYING);

            refresh(true);
        }
        else
        {
            display_already_scored();
        }
    }

    void action_score_best(player *player)
    {
        auto best_upper = player->calculate_best_upper_score();

        auto best_lower = player->calculate_best_lower_score();

        if (best_upper.second > best_lower.second)
        {

            if (!player->score().upper_score(best_upper.first))
            {
                player->score().upper_score(best_upper.first, best_upper.second);

                set_state(PLAYING);

                refresh(true);
            }
            else
            {
                display_already_scored();
            }

        }
        else if (best_lower.second > 0)
        {
            if (!player->score().lower_score(best_lower.first))
            {
                player->score().lower_score(best_lower.first, best_lower.second);

                set_state(PLAYING);

                refresh(true);
            }
            else
            {
                display_already_scored();
            }
        }
        else
        {
            display_alert([&](const alert_box & box)
            {
                box.center("No best score found!");
            });
            add_event(2000, [&]()
            {
                pop_alert();

                action_display_dice();
            });
        }
    }

    void action_confirm_quit()
    {
        display_alert([&](const alert_box & box)
        {
            box.center("Are you sure you want to quit? (Y/n)");
        });
    }

    void on_resize(int width, int height)
    {
    }

    void on_quit()
    {
        state_ = QUIT;
    }

    void on_key_press(int input)
    {
        switch (state())
        {
        case ASK_NAME:
            if ( input == CACA_KEY_ESCAPE)
            {
                set_state(QUIT);
            }
            else if (isnumber(input) || isalpha(input) || input == CACA_KEY_RETURN)
            {
                action_ask_name(input);
            }
            break;

        case DISPLAY_MENU:
            if (input == CACA_KEY_ESCAPE || tolower(input) == 'q')
            {
                recover_state();
                pop_alert();
                refresh(state_ == PLAYING);
            }
            break;

        case QUIT_CONFIRM:
            if (tolower(input) == 'n')
            {
                pop_alert();
                recover_state();
                refresh(state_ == PLAYING);
            }
            else
            {
                set_state(QUIT);
            }
            break;

        case ROLLING_DICE:
            handle_rolling_command(input);
            break;
        default:
            handle_normal_command(input);
            break;

        }
    }

protected:

    void handle_normal_command(int input)
    {
        // check non ascii commands
        switch (input)
        {
        case CACA_KEY_UP:
        {
            int mode = display_mode_;
            if (mode == MINIMAL)
                display_mode_ = HORIZONTAL;
            else
                display_mode_ = static_cast<display_mode>(++mode);
            refresh(true);
            return;
        }
        case CACA_KEY_DOWN:
        {
            int mode = display_mode_;
            if (mode == HORIZONTAL)
                display_mode_ = MINIMAL;
            else
                display_mode_ = static_cast<display_mode>(--mode);
            refresh(true);
            return;
        }
        case CACA_KEY_LEFT:
        case CACA_KEY_RIGHT:
            if (display_mode_ == MINIMAL)
            {
                minimalLower_ = !minimalLower_;
                refresh(true);
            }
            return;

        case CACA_KEY_ESCAPE:
            set_state(QUIT_CONFIRM);
            action_confirm_quit();
            return;
        }

        //check ascii commands in lower case
        switch (tolower(input))
        {
        case 'r':
            set_state(ROLLING_DICE);
            action_roll_dice();
            break;
        case '?':
            set_state(DISPLAY_MENU);
            display_menu();
            break;
        case 'q':
            set_state(QUIT_CONFIRM);
            action_confirm_quit();
            break;
        default: break;

        }
    }
    void handle_rolling_command(int input)
    {
        if (input == CACA_KEY_ESCAPE || tolower(input) == 'q')
        {
            set_state(PLAYING);
            pop_alert();
            refresh(true);
        }

        auto player = engine::instance()->current_player();

        switch (tolower(input))
        {

        case '?':
            set_state(DISPLAY_MENU);
            display_menu();
            break;
        case 'r':
            set_state(ROLLING_DICE);
            action_roll_dice();
            break;
        case 'f':
            action_lower_score(player, scoresheet::FULL_HOUSE);
            break;
        case 'k':
        case 't':
        case 's':
            add_to_buffer(input);
            break;
        case 'y':
            action_lower_score(player, scoresheet::YACHT);
            break;
        case 'c':
            action_lower_score(player, scoresheet::CHANCE);
            break;
        case 'b':
            if (state() == ROLLING_DICE)
            {
                auto buffer = get_buffer();

                if (buffer.length() > 0)
                {
                    switch (tolower(buffer[0]))
                    {
                    case 's':
                        action_score_best(player);
                        break;
                    }

                    buffer.clear();
                }
            }
            break;
        default:

            if (isdigit(input))
            {
                auto buffer = get_buffer();

                if (buffer.length() > 0)
                {
                    switch (tolower(buffer[0]))
                    {
                    case 'k':
                        if (input == '3')
                        {
                            action_lower_score(player, scoresheet::KIND_THREE);
                        }
                        else if (input == '4')
                        {
                            action_lower_score(player, scoresheet::KIND_FOUR);
                        }
                        break;
                    case 's':
                        action_score(player, input - '0');
                        break;
                    case 't':
                        if (input == '4')
                        {
                            action_lower_score(player, scoresheet::STRAIGHT_SMALL);
                        }
                        else if (input == '5')
                        {
                            action_lower_score(player, scoresheet::STRAIGHT_BIG);
                        }
                        break;
                    }

                    clear_buffer();
                }
                else
                {
                    action_select_die(player, input - '0' - 1);
                }
            }
            break;
        }
    }

    void init_canvas(caca_canvas_t *canvas)
    {
        caca_canvas_t *temp = caca_create_canvas(0, 0);

        caca_import_canvas_from_file(temp, "upper.txt", "utf8");

        upperbuf_ = caca_export_canvas_to_memory(temp, "caca", &upperbuf_size_);

        caca_free_canvas(temp);

        temp = caca_create_canvas(0, 0);

        caca_import_canvas_from_file(temp, "lower.txt", "utf8");

        lowerbuf_ = caca_export_canvas_to_memory(temp, "caca", &lowerbuf_size_);

        caca_free_canvas(temp);

        temp = caca_create_canvas(0, 0);

        caca_import_canvas_from_file(temp, "menu.txt", "utf8");

        menubuf_ = caca_export_canvas_to_memory(temp, "caca", &menubuf_size_);

        caca_free_canvas(temp);

        temp = caca_create_canvas(0, 0);

        caca_import_canvas_from_file(temp, "header.txt", "utf8");

        headerbuf_ = caca_export_canvas_to_memory(temp, "caca", &headerbuf_size_);

        caca_free_canvas(temp);

        switch (display_mode_)
        {
        case VERTICAL:
            caca_import_area_from_memory(canvas, 0, 0, headerbuf_, headerbuf_size_, "caca");
            caca_import_area_from_memory(canvas, 0, 8, upperbuf_, upperbuf_size_, "caca");
            caca_import_area_from_memory(canvas, 0, 27, lowerbuf_, lowerbuf_size_, "caca");
            break;
        case HORIZONTAL:

            caca_import_area_from_memory(canvas, 0, 0, headerbuf_, headerbuf_size_, "caca");
            caca_import_area_from_memory(canvas, 0, 8, upperbuf_, upperbuf_size_, "caca");
            caca_import_area_from_memory(canvas, 76, 1, lowerbuf_, lowerbuf_size_, "caca");
            break;
        case MINIMAL:
            caca_import_area_from_memory(canvas, 0, 0, headerbuf_, headerbuf_size_, "caca");
            caca_import_area_from_memory(canvas, 0, 8, minimalLower_ ? lowerbuf_ : upperbuf_, minimalLower_ ? lowerbuf_size_ : upperbuf_size_, "caca");
            break;
        }
    }
    void set_display_mode(display_mode mode)
    {
        display_mode_ = mode;
    }
private:

    int get_alert_x() const
    {
        switch (display_mode_)
        {
        default:
            return 8;
        case HORIZONTAL:
            return 48;
        }
    }

    int get_alert_y() const
    {
        switch (display_mode_)
        {
        case VERTICAL:
            return 20;
        case MINIMAL:
        case HORIZONTAL: return 8;
        }
    }

    int get_alert_w() const
    {
        return 60;
    }
    int get_alert_h() const
    {
        return 15;
    }

    void display_alert(function<void(const alert_box &a)> funk)
    {
        caca_game::display_alert(get_alert_x(), get_alert_y(), get_alert_w(), get_alert_h(), funk);
    }

    void display_menu()
    {
        display_alert([&](const alert_box & a)
        {
            caca_import_area_from_memory(a.canvas(), a.x() + 4, a.y() + 3, menubuf_, menubuf_size_, "caca");
        });
    }

    scoresheet::value_type display_upper_scores(const scoresheet &score, int x, int y)
    {
        scoresheet::value_type total_score = 0;

        for (int i = 0; i <= Constants::NUM_DICE; i++, y += 2)
        {
            auto value = score.upper_score(i + 1);

            put(x, y, std::to_string(value).c_str());

            total_score += value;
        }

        put(x, y, std::to_string(total_score).c_str());

        put(x, y + 2, std::to_string(total_score > 63 ? 35 : 0).c_str());

        auto lower_score_total = total_score;

        if (total_score > 63)
            lower_score_total += 35;

        put(x, y + 4, std::to_string(lower_score_total).c_str());

        return lower_score_total;

    }

    void display_lower_scores(const scoresheet &score, scoresheet::value_type lower_score_total, int x, int y)
    {
        scoresheet::value_type total_score = 0;

        for (int i = scoresheet::FIRST_TYPE; i < scoresheet::MAX_TYPE; i++)
        {
            scoresheet::type type = static_cast<scoresheet::type>(i);

            auto value = score.lower_score(type);

            put(x, y, std::to_string(value).c_str());

            total_score += value;

            switch (type)
            {
            default:
                y += 2;
                break;
            case scoresheet::STRAIGHT_SMALL:
            case scoresheet::STRAIGHT_BIG:
            case scoresheet::YACHT:
                y += 3;
                break;
            }
        }

        put(x, y, std::to_string(total_score).c_str());

        put(x, y + 2, std::to_string(lower_score_total).c_str());

        put(x, y + 4, std::to_string(total_score + lower_score_total).c_str());
    }

    void *upperbuf_, *lowerbuf_, *menubuf_, *headerbuf_;
    size_t upperbuf_size_, lowerbuf_size_, menubuf_size_, headerbuf_size_;
    game_state state_, last_state_;
    display_mode display_mode_;
    bool minimalLower_;
};
