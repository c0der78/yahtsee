
#include <stdlib.h>
#include <iostream>
#include <arg3dice/yaht/engine.h>

#include "caca.h"

#include "yaht_game.h"

using namespace std;

void display_dice(caca_game &, player *);

int main(int argc, char **argv)
{
    yaht_game game;

    game.start();

    while (game.alive())
    {
        if (!game.has_key_press())
            continue;

        if (game.state() == ASK_NAME)
        {
            int ch = game.read_input();

            if (ch == CACA_KEY_RETURN)
            {
                string name = game.get_buffer();

                engine::instance()->add_player(name);

                game.set_state(PLAYING);

                game.clear_buffer();

                game.clear();
            }
            else
            {
                int len = game.add_to_buffer(ch);

                game.put(80 + len - 1, 22, ch);

                game.set_cursor(80 + len, 22);
            }

            game.refresh();

            continue;
        }

        auto player = engine::instance()->current_player();

        char input = game.read_input();

        switch (input)
        {
        case 'r':
            if (player->roll_count() < 3)
            {
                player->roll();
                game.new_frame();
                display_dice(game, player);
                game.refresh();
            }
            else
            {
                game.put(80, 28, "You must choose a score after three rolls.");
                game.refresh();
            }
            break;
        case 'f':
            player->score().lower_score(scoresheet::FULL_HOUSE, player->calculate_lower_score(scoresheet::FULL_HOUSE));
            game.refresh(true);
            break;
        case 'k':
        case 't':
        case 's':
            game.add_to_buffer(input);
            break;
        case 'y':
            player->score().lower_score(scoresheet::YACHT, player->calculate_lower_score(scoresheet::YACHT));
            game.refresh(true);
            break;
        case 'c':
            player->score().lower_score(scoresheet::CHANCE, player->calculate_lower_score(scoresheet::CHANCE));
            game.refresh(true);
            break;

        case 'q':
            game.set_state(QUIT);
            break;
        default:

            if (input >= '0' && input <= '9')
            {
                auto buffer = game.get_buffer();

                if (buffer.length() > 0)
                {
                    switch (buffer[0])
                    {
                    case 'k':
                    {
                        if (input == '3')
                        {
                            player->score().lower_score(scoresheet::KIND_THREE, player->calculate_lower_score(scoresheet::KIND_THREE));
                            game.refresh(true);
                        }
                        else if (input == '4')
                        {
                            player->score().lower_score(scoresheet::KIND_FOUR, player->calculate_lower_score(scoresheet::KIND_FOUR));
                            game.refresh(true);
                        }
                        break;
                    }
                    case 's':
                    {
                        int n = input - '0';

                        player->score().upper_score(n, player->calculate_upper_score(n));

                        game.refresh(true);
                        break;
                    }
                    case 't':
                    {
                        if (input == '4')
                        {
                            player->score().lower_score(scoresheet::STRAIGHT_SMALL, player->calculate_lower_score(scoresheet::STRAIGHT_SMALL));
                            game.refresh(true);
                        }
                        else if (input == '5')
                        {
                            player->score().lower_score(scoresheet::STRAIGHT_BIG, player->calculate_lower_score(scoresheet::STRAIGHT_BIG));
                            game.refresh(true);
                        }

                    }
                    }

                    game.clear_buffer();
                }
                else
                {
                    int d = input - '0' - 1;
                    if (player->is_kept(d))
                        player->keep_die(d, false);
                    else
                        player->keep_die(d, true);
                    display_dice(game, player);
                    game.refresh();
                }
            }
        }
    }

    return 0;
}

void display_dice(caca_game &game, player *player)
{
    char buf[BUFSIZ + 1] = {0};
    snprintf(buf, BUFSIZ, "Roll %d of 3. (Use '#' to keep):", player->roll_count());
    game.put(80, 20, buf);
    int x = 82;

    game.put(80, 22, "#  1 │ 2 │ 3 │ 4 │ 5");
    game.put(80, 23, "  ───┴───┴───┴───┴───");
    game.put(80, 24, "  ");
    for (size_t i = 0; i < player->die_count(); i++)
    {
        game.put(x++, 24, player->is_kept(i) ? '*' : ' ');

        game.put(x++, 24, to_string(player->d1e(i).value()).c_str());

        x += 2;
    }
}
