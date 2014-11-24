#include "yaht_game.h"

void yaht_game::state_ask_name(int ch)
{
    if ( ch == CACA_KEY_ESCAPE)
    {
        set_state(&yaht_game::state_game_menu);
        display_game_menu();
        return;
    }

    if (ch == CACA_KEY_RETURN || ch == 10)
    {
        string name = get_buffer();

        yaht_.add_player(make_shared<arg3::yaht::player>(name));

        pop_alert();

        clear();

        if (flags_ & FLAG_HOSTING)
        {
            action_host_game();
        }
        else if (flags_ & FLAG_JOINING)
        {
            action_join_game();
        }
        else if (yaht_.number_of_players() >= num_players_)
        {
            set_state(&yaht_game::state_playing);
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

void yaht_game::state_help_menu(int ch)
{
    if (ch == CACA_KEY_ESCAPE || tolower(ch) == 'q')
    {
        recover_state();
        pop_alert();
        refresh(is_playing());
    }
}

void yaht_game::state_game_menu(int input)
{
    if (input == CACA_KEY_ESCAPE || tolower(input) == 'q')
    {
        set_state(&yaht_game::state_quit_confirm);
        display_confirm_quit();
        return;
    }

    switch (tolower(input))
    {
    case 'n':
        set_state(&yaht_game::state_ask_number_of_players);
        display_ask_number_of_players();
        break;
    case 'm':
        set_state(&yaht_game::state_multiplayer_menu);
        display_multiplayer_menu();
        break;
    case 's':
        break;
    }
}

void yaht_game::state_multiplayer_menu(int input)
{
    if (input == CACA_KEY_ESCAPE)
    {
        pop_alert();
        set_state(&yaht_game::state_game_menu);
        display_game_menu();
        return;
    }

    switch (tolower(input))
    {
    case 'h':
        flags_ |= FLAG_HOSTING;
        set_state(&yaht_game::state_ask_name);
        display_ask_name();
        break;
    case 'j':
        flags_ |= FLAG_JOINING;
        set_state(&yaht_game::state_ask_name);
        display_ask_name();
        break;
    }
}

void yaht_game::state_ask_number_of_players(int input)
{
    if (isdigit(input))
    {
        num_players_ = input - '0';

        set_state(&yaht_game::state_ask_name);
        display_ask_name();
    }
}

void yaht_game::state_waiting_for_connections(int input)
{
    if (input == CACA_KEY_ESCAPE || tolower(input) == 'q')
    {
        pop_alert();
        set_state(&yaht_game::state_multiplayer_menu);
        display_multiplayer_menu();
        matchmaker_.stop();
    }
    else if (tolower(input) == 's' && yaht_.number_of_players() > 1)
    {
        set_state(&yaht_game::state_playing);

        matchmaker_.notify_game_start();

        refresh(true);
    }
}

void yaht_game::state_quit_confirm(int input)
{
    if (tolower(input) == 'n')
    {
        pop_alert();
        recover_state();
        refresh(is_playing());
    }
    else
    {
        set_state(nullptr);
    }
}


void yaht_game::state_playing(int input)
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
        set_state(&yaht_game::state_quit_confirm);
        display_confirm_quit();
        return;
    }

    //check ascii commands in lower case
    switch (tolower(input))
    {
    case 'r':
        set_state(&yaht_game::state_rolling_dice);
        action_roll_dice();
        break;
    case '?':
        set_state(&yaht_game::state_help_menu);
        display_help();
        break;
    case 'q':
        set_state(&yaht_game::state_quit_confirm);
        display_confirm_quit();
        break;
    default: break;

    }
}


void yaht_game::state_rolling_dice(int input)
{
    if (input == CACA_KEY_ESCAPE || tolower(input) == 'q')
    {
        set_state(&yaht_game::state_playing);
        pop_alert();
        refresh(true);
    }

    auto player = yaht_.current_player();

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
        return;
    }

    switch (tolower(input))
    {

    case '?':
        set_state(&yaht_game::state_help_menu);
        display_help();
        break;
    case 'r':
        set_state(&yaht_game::state_rolling_dice);
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
        if (is_state(&yaht_game::state_rolling_dice))
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

