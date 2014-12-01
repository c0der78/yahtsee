#include "game.h"
#include "player.h"

using namespace arg3;

void game::state_ask_name(int ch)
{
    if ( ch == CACA_KEY_ESCAPE)
    {
        set_state(&game::state_game_menu);
        display_game_menu();
        return;
    }

    if (ch == CACA_KEY_RETURN || ch == 10)
    {
        string name = get_buffer();

        players_.push_back(make_shared<player>(name));

        clear();

        if (flags_ & FLAG_HOSTING)
        {
            action_host_game();
        }
        else if (flags_ & FLAG_JOINING)
        {
            action_join_game();
        }
        else if (players_.size() >= numPlayers_)
        {
            set_state(&game::state_playing);
        }
        else
        {
            display_ask_name();
        }

        clear_buffer();
    }
    else if (isalnum(ch))
    {
        add_to_buffer(ch);

        int x = get_cursor_x();

        int y = get_cursor_y();

        put(x + 1, y, ch);

        set_cursor(x + 1, y);
    }

    refresh();
}

void game::state_help_menu(int ch)
{
    if (ch == CACA_KEY_ESCAPE || tolower(ch) == 'q')
    {
        recover_state();
        refresh(is_playing());
    }
}

void game::state_game_menu(int input)
{
    if (input == CACA_KEY_ESCAPE || tolower(input) == 'q')
    {
        set_state(&game::state_quit_confirm);
        display_confirm_quit();
        return;
    }

    switch (tolower(input))
    {
    case 'n':
        set_state(&game::state_ask_number_of_players);
        display_ask_number_of_players();
        break;
    case 'm':
        set_state(&game::state_multiplayer_menu);
        display_multiplayer_menu();
        break;
    case 's':
        break;
    }
}

void game::state_multiplayer_menu(int input)
{
    if (input == CACA_KEY_ESCAPE)
    {
        set_state(&game::state_game_menu);
        display_game_menu();
        return;
    }

    switch (tolower(input))
    {
    case 'h':
        flags_ |= FLAG_HOSTING;
        set_state(&game::state_ask_name);
        display_ask_name();
        break;
    case 'j':
        flags_ |= FLAG_JOINING;
        set_state(&game::state_ask_name);
        display_ask_name();
        break;
    }
}

void game::state_ask_number_of_players(int input)
{
    if (isdigit(input))
    {
        numPlayers_ = input - '0';

        if (numPlayers_ > 6)
        {
            display_alert(2000, "A game can have up to 6 players only.");
            return;
        }

        set_state(&game::state_ask_name);
        display_ask_name();
    }
}

void game::state_waiting_for_connections(int input)
{
    if (input == CACA_KEY_ESCAPE || tolower(input) == 'q')
    {
        flags_ = 0;
        matchmaker_.stop();
        set_state(&game::state_multiplayer_menu);
        display_multiplayer_menu();
    }
    else if (tolower(input) == 's' && players_.size() > 1)
    {
        set_state(&game::state_playing);

        matchmaker_.notify_game_start();

        refresh(true);
    }
}

void game::state_quit_confirm(int input)
{
    if (tolower(input) == 'n')
    {
        recover_state();
        refresh(is_playing());
    }
    else
    {
        set_state(nullptr);
    }
}


void game::state_playing(int input)
{
    // check non ascii commands
    switch (input)
    {
    case CACA_KEY_UP:
    {
        int mode = displayMode_;
        if (mode == MINIMAL)
            displayMode_ = HORIZONTAL;
        else
            displayMode_ = static_cast<display_mode>(++mode);
        refresh(true);
        return;
    }
    case CACA_KEY_DOWN:
    {
        int mode = displayMode_;
        if (mode == HORIZONTAL)
            displayMode_ = MINIMAL;
        else
            displayMode_ = static_cast<display_mode>(--mode);
        refresh(true);
        return;
    }
    case CACA_KEY_LEFT:
    case CACA_KEY_RIGHT:
        if (displayMode_ == MINIMAL)
        {
            minimalLower_ = !minimalLower_;
            refresh(true);
        }
        return;

    case CACA_KEY_ESCAPE:
        set_state(&game::state_quit_confirm);
        display_confirm_quit();
        return;
    }

    //check ascii commands in lower case
    switch (tolower(input))
    {
    case 'r':
        set_state(&game::state_rolling_dice);
        action_roll_dice();
        break;
    case '?':
        set_state(&game::state_help_menu);
        display_help();
        break;
    case 'q':
        set_state(&game::state_quit_confirm);
        display_confirm_quit();
        break;
    default: break;

    }
}


void game::state_rolling_dice(int input)
{
    if (input == CACA_KEY_ESCAPE || tolower(input) == 'q')
    {
        set_state(&game::state_playing);
        refresh(true);
    }

    auto player = current_player();

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
                    action_lower_score(player, yaht::scoresheet::KIND_THREE);
                }
                else if (input == '4')
                {
                    action_lower_score(player, yaht::scoresheet::KIND_FOUR);
                }
                break;
            case 's':
                action_score(player, input - '0');
                break;
            case 't':
                if (input == '4')
                {
                    action_lower_score(player, yaht::scoresheet::STRAIGHT_SMALL);
                }
                else if (input == '5')
                {
                    action_lower_score(player, yaht::scoresheet::STRAIGHT_BIG);
                }
                break;
            }

            clear_buffer();
        }
        else
        {
            action_select_die(player, input - '0' - 1);
        }
        return;
    }

    switch (tolower(input))
    {

    case '?':
        set_state(&game::state_help_menu);
        display_help();
        break;
    case 'r':
        set_state(&game::state_rolling_dice);
        action_roll_dice();
        break;
    case 'f':
        action_lower_score(player, yaht::scoresheet::FULL_HOUSE);
        break;
    case 'k':
    case 't':
    case 's':
        add_to_buffer(input);
        break;
    case 'y':
        action_lower_score(player, yaht::scoresheet::YACHT);
        break;
    case 'c':
        action_lower_score(player, yaht::scoresheet::CHANCE);
        break;
    case 'b':
        if (is_state(&game::state_rolling_dice))
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
    }
}

