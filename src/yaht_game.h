
#include "caca_game.h"

#include "yaht/engine.h"

using namespace arg3::yaht;

typedef enum
{
    ASK_NAME,
    PLAYING,
    QUIT
} game_state;

class yaht_game : public caca_game
{
public:
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
            put(80, 20, "What is your name? ");
            set_cursor(80, 22);
            break;

        default:
            put(80, 20, "Command: ");
            set_cursor(80, 22);
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

            total_score = 0;

            y  += 8;

            for (int i = scoresheet::FIRST_TYPE; i < scoresheet::MAX_TYPE; i++)
            {
                scoresheet::type type = static_cast<scoresheet::type>(i);

                auto score = player->score().lower_score(type);

                put(46, y, std::to_string(score).c_str());

                total_score += score;

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

            put(46, y, std::to_string(total_score).c_str());

            put(46, y + 2, std::to_string(lower_score_total).c_str());

            put(46, y + 4, std::to_string(total_score + lower_score_total).c_str());
        }

    }


    void display_already_scored()
    {
        put(80, 28, "You've already scored that.");
        refresh();
    }

    void display_dice(player *player)
    {
        char buf[BUFSIZ + 1] = {0};
        snprintf(buf, BUFSIZ, "Roll %d of 3. (Use '#' to keep):", player->roll_count());
        put(80, 20, buf);
        int x = 82;

        put(80, 22, "#  1 │ 2 │ 3 │ 4 │ 5");
        put(80, 23, "  ───┴───┴───┴───┴───");
        put(80, 24, "  ");
        for (size_t i = 0; i < player->die_count(); i++)
        {
            put(x++, 24, player->is_kept(i) ? '*' : ' ');

            put(x++, 24, to_string(player->d1e(i).value()).c_str());

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

            clear_buffer();

            clear();
        }
        else
        {
            int len = add_to_buffer(ch);

            put(80 + len - 1, 22, ch);

            set_cursor(80 + len, 22);
        }

        refresh();
    }


    void action_select_die(player *player, int d)
    {
        if (player->is_kept(d))
            player->keep_die(d, false);
        else
            player->keep_die(d, true);

        display_dice(player);
        refresh();
    }

    void action_roll(player *player)
    {
        if (player->roll_count() < 3)
        {
            player->roll();
            new_frame();
            display_dice(player);
            refresh();
        }
        else
        {
            put(80, 28, "You must choose a score after three rolls.");
            refresh();
        }
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
            state_ask_name(input);

            return;
        }

        auto player = engine::instance()->current_player();

        switch (tolower(input))
        {
        case 'r':
            action_roll(player);
            break;
        case 'f':
            action_full_house(player);
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

private:

    game_state state_;
};
