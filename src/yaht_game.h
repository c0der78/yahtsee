
#include "caca_game.h"

#include <arg3dice/yaht/engine.h>

using namespace arg3::yaht;

typedef enum
{
    ASK_NAME,
    PLAYING,
    ROLLING_DICE,
    DISPLAY_MENU,
    QUIT
} game_state;

typedef enum
{
    NORMAL,
    HORIZONTAL,
    VERTICAL,
    MINIMAL
} display_mode;

#define MENU_W 60
#define MENU_H 15
#define MENU_X 8
#define MENU_Y 20

class yaht_game : public caca_game
{
public:
    yaht_game() : upperbuf_(NULL), lowerbuf_(NULL), menubuf_(NULL), upperbuf_size_(0), lowerbuf_size_(0), menubuf_size_(0), display_mode_(NORMAL)
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

    void set_state(game_state value)
    {
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
            display_alert(MENU_X, MENU_Y, MENU_W, MENU_H, [&](const alert_box &)
            {
                put(x + (MENU_W / 2) - 10, y + (MENU_H / 2) - 1, "What is your name? ");
                set_cursor(x + (MENU_W / 2) - 10, y + (MENU_H / 2));
            });
            break;
        case ROLLING_DICE:
            display_alert(MENU_X, MENU_Y, MENU_W, MENU_H, [&](const alert_box & box)
            {

                player *player = engine::instance()->current_player();

                if (player->roll_count() < 3)
                {
                    player->roll();
                    new_frame();
                    display_dice(player, box.x(), box.y());
                    refresh();
                }
                else
                {
                    box.center("You must choose a score after three rolls.");
                }
                refresh();
            });
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
        if (has_alert()) return;

        player *player = engine::instance()->current_player();

        if (player != NULL)
        {
            if (reset)
                player->reset();

            put(50, 2, player->name().c_str());

            int y = 8;

            scoresheet::value_type total_score = 0;

            for (int i = 0; i <= Constants::NUM_DICE; i++, y += 2)
            {
                auto score = player->score().upper_score(i + 1);

                put(46, y, std::to_string(score).c_str());

                total_score += score;
            }

            put(46, y, std::to_string(total_score).c_str());

            put(46, y + 2, std::to_string(total_score > 63 ? 35 : 0).c_str());

            auto lower_score_total = total_score;

            if (total_score > 63)
                lower_score_total += 35;

            put(46, y + 4, std::to_string(lower_score_total).c_str());

            switch (display_mode_)
            {
            default:
                display_lower_scores(player->score(), lower_score_total, 46, y + 8);
                break;
            case HORIZONTAL:
                display_lower_scores(player->score(), lower_score_total, 122, 3);
            }

        }

    }


    void display_already_scored()
    {
        alert_boxes
        put(80, 28, "You've already scored that.");
        refresh();
    }

    void display_dice(player *player, int x, int y)
    {
        char buf[BUFSIZ + 1] = {0};
        snprintf(buf, BUFSIZ, "Roll %d of 3. (Use '#' to keep):", player->roll_count());

        x += 13;
        y += 5;

        put(x, y, buf);

        x += 2;
        y += 2;

        put(x, y++, "#  1 │ 2 │ 3 │ 4 │ 5");
        put(x, y++, "  ───┴───┴───┴───┴───");
        put(x, y++, "  ");
        for (size_t i = 0; i < player->die_count(); i++)
        {
            put(x++, y, player->is_kept(i) ? '*' : ' ');

            put(x++, y, to_string(player->d1e(i).value()).c_str());

            x += 2;
        }
    }

    void state_ask_name(int ch)
    {
        if (ch == CACA_KEY_RETURN)
        {
            string name = get_buffer();

            engine::instance()->add_player(name);

            set_state(PLAYING);

            alert_box_ = nullptr;

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


    void action_select_die(player *player, int d)
    {
        if (player->is_kept(d))
            player->keep_die(d, false);
        else
            player->keep_die(d, true);

        display_dice(player, MENU_X, MENU_Y);
        refresh();
    }


    void action_full_house(player *player)
    {
        if (!player->score().lower_score(scoresheet::FULL_HOUSE))
        {
            player->score().lower_score(scoresheet::FULL_HOUSE, player->calculate_lower_score(scoresheet::FULL_HOUSE));
            refresh(true);
        }
        else
        {
            display_already_scored();
        }
    }

    void action_yaht(player *player)
    {
        if (!player->score().lower_score(scoresheet::YACHT))
        {
            player->score().lower_score(scoresheet::YACHT, player->calculate_lower_score(scoresheet::YACHT));
            refresh(true);
        }
        else
        {
            display_already_scored();
        }
    }

    void action_chance(player *player)
    {
        if (!player->score().lower_score(scoresheet::CHANCE))
        {
            player->score().lower_score(scoresheet::CHANCE, player->calculate_lower_score(scoresheet::CHANCE));
            refresh(true);
        }
        else
        {
            display_already_scored();
        }
    }

    void action_three_of_a_kind(player *player)
    {
        if (!player->score().lower_score(scoresheet::KIND_THREE))
        {
            player->score().lower_score(scoresheet::KIND_THREE, player->calculate_lower_score(scoresheet::KIND_THREE));
            refresh(true);
        }
        else
        {
            display_already_scored();
        }
    }

    void action_four_of_a_kind(player *player)
    {
        if (!player->score().lower_score(scoresheet::KIND_FOUR))
        {
            player->score().lower_score(scoresheet::KIND_FOUR, player->calculate_lower_score(scoresheet::KIND_FOUR));
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

            refresh(true);
        }
        else
        {
            display_already_scored();
        }
    }

    void action_small_straight(player *player)
    {
        if (!player->score().lower_score(scoresheet::STRAIGHT_SMALL))
        {
            player->score().lower_score(scoresheet::STRAIGHT_SMALL, player->calculate_lower_score(scoresheet::STRAIGHT_SMALL));
            refresh(true);
        }
        else
        {
            display_already_scored();
        }
    }

    void action_large_straight(player *player)
    {
        if (!player->score().lower_score(scoresheet::STRAIGHT_BIG))
        {
            player->score().lower_score(scoresheet::STRAIGHT_BIG, player->calculate_lower_score(scoresheet::STRAIGHT_BIG));
            refresh(true);
        }
        else
        {
            display_already_scored();
        }
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
        if (state() == ASK_NAME)
        {
            if ( input == CACA_KEY_ESCAPE)
            {
                set_state(QUIT);
            }
            else
            {
                state_ask_name(input);
            }
            return;
        }

        if (state() == DISPLAY_MENU)
        {
            if (input == CACA_KEY_ESCAPE || tolower(input) == 'q')
            {
                set_state(PLAYING);
                alert_box_ = nullptr;
                refresh(true);
            }
            return;
        }

        auto player = engine::instance()->current_player();

        switch (tolower(input))
        {
        case 'r':
            alert_box_ = alert(MENU_W, MENU_H, [&](const alert_box & box, int x, int y)
            {
                action_roll(player, x, y);
            });
            alert_box_->display(MENU_X, MENU_Y);
            break;
        case 'f':
            action_full_house(player);
            break;
        case CACA_KEY_UP:
        {
            int mode = display_mode_;
            if (++mode > MINIMAL)
                display_mode_ = NORMAL;
            else
                display_mode_ = static_cast<display_mode>(mode);
            refresh(true);
            break;
        }
        case CACA_KEY_DOWN:
        {
            int mode = display_mode_;
            if (--mode < NORMAL)
                display_mode_ = MINIMAL;
            else
                display_mode_ = static_cast<display_mode>(mode);
            refresh(true);
            break;
        }
        case '?':
            display_menu();
            break;
        case 'k':
        case 't':
        case 's':
            add_to_buffer(input);
            break;
        case 'y':
            action_yaht(player);
            break;
        case 'c':
            action_chance(player);
            break;
        case CACA_KEY_ESCAPE:
        case 'q':
            set_state(QUIT);
            break;
        default:

            if (input >= '0' && input <= '9')
            {
                auto buffer = get_buffer();

                if (buffer.length() > 0)
                {
                    switch (tolower(buffer[0]))
                    {
                    case 'k':
                    {
                        if (input == '3')
                        {
                            action_three_of_a_kind(player);
                        }
                        else if (input == '4')
                        {
                            action_four_of_a_kind(player);
                        }
                        break;
                    }
                    case 's':
                    {
                        action_score(player, input - '0');
                        break;
                    }
                    case 't':
                    {
                        if (input == '4')
                        {
                            action_small_straight(player);
                        }
                        else if (input == '5')
                        {
                            action_large_straight(player);
                        }

                    }
                    }

                    clear_buffer();
                }
                else
                {
                    action_select_die(player, input - '0' - 1);
                }
            }
        }
    }

protected:

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

        switch (display_mode_)
        {
        default:
            caca_import_area_from_memory(canvas, 0, 0, upperbuf_, upperbuf_size_, "caca");
            caca_import_area_from_memory(canvas, 0, 27, lowerbuf_, lowerbuf_size_, "caca");
            break;
        case HORIZONTAL:
            caca_import_area_from_memory(canvas, 0, 0, upperbuf_, upperbuf_size_, "caca");
            caca_import_area_from_memory(canvas, 76, 0, lowerbuf_, lowerbuf_size_, "caca");
            break;
        case VERTICAL:
            caca_import_area_from_memory(canvas, 0, 0, upperbuf_, upperbuf_size_, "caca");
            caca_import_area_from_memory(canvas, 0, 27, lowerbuf_, lowerbuf_size_, "caca");
            break;
        }
    }
    void set_display_mode(display_mode mode)
    {
        display_mode_ = mode;
    }
private:

    void display_menu()
    {
        state_ = DISPLAY_MENU;

        alert_box_ = alert(MENU_W, MENU_H, [&](const alert_box & box, int x, int y)
        {
            caca_import_area_from_memory(box.canvas(), x + 4, y + 3, menubuf_, menubuf_size_, "caca");
        });

        alert_box_->display(MENU_X, MENU_Y);
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

    void *upperbuf_, *lowerbuf_, *menubuf_;
    size_t upperbuf_size_, lowerbuf_size_, menubuf_size_;
    game_state state_;
    display_mode display_mode_;
};
