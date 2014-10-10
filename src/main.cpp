
#include <stdlib.h>
#include <iostream>
#include "yaht/engine.h"

#include "caca.h"

#include "yaht_game.h"

using namespace std;


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
            game.state_ask_name();

            continue;
        }

        auto player = engine::instance()->current_player();

        char input = game.read_input();

        switch (tolower(input))
        {
        case 'r':
            game.action_roll(player);
            break;
        case 'f':
            game.action_full_house(player);
            break;
        case 'k':
        case 't':
        case 's':
            game.add_to_buffer(input);
            break;
        case 'y':
            game.action_yaht(player);
            break;
        case 'c':
            game.action_chance(player);
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
                    switch (tolower(buffer[0]))
                    {
                    case 'k':
                    {
                        if (input == '3')
                        {
                            game.action_three_of_a_kind(player);
                        }
                        else if (input == '4')
                        {
                            game.action_four_of_a_kind(player);
                        }
                        break;
                    }
                    case 's':
                    {
                        game.action_score(player, input - '0');
                        break;
                    }
                    case 't':
                    {
                        if (input == '4')
                        {
                            game.action_small_straight(player);
                        }
                        else if (input == '5')
                        {
                            game.action_large_straight(player);
                        }

                    }
                    }

                    game.clear_buffer();
                }
                else
                {
                    game.action_select_die(player, input - '0' - 1);
                }
            }
        }
    }

    return 0;
}

