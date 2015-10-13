#include "game.h"
#include "player.h"
#include "log.h"
#include <arg3/str_util.h>

using namespace arg3;

void game::state_ask_name(int ch)
{
    if (ch == CACA_KEY_RETURN || ch == 10)
    {
        string name = get_buffer();

        if (name.empty()) { return; }

        players_.push_back(make_shared<player>(capitalize(name)));

        if (flags_ & FLAG_HOSTING)
        {
            action_host_game();
        }
        else if (flags_ & FLAG_JOINING)
        {
            if (flags_ & FLAG_LAN) {
                set_state(&game::state_joining_game);
            } else {
                set_state(&game::state_joining_online_game);
            }
        }
        else if (players_.size() >= numPlayers_)
        {
            set_state(&game::state_playing);

            set_needs_clear();
        }
        else
        {
            display_ask_name();
        }

        clear_buffer();
    }
    else if (ch == 8 || ch == 127) {
        pop_from_buffer();
    }
    else if (isprint(ch))
    {
        add_to_buffer(ch);
    }

    set_needs_display();
}

void game::state_joining_game(int ch)
{

}

void game::state_joining_online_game(int ch)
{

}

void game::state_hosting_game(int ch)
{
    switch(tolower(ch))
    {
        case 'q':
        set_state(&game::state_quit_confirm);
        break;
    }
}

void game::state_help_menu(int ch)
{
    if (tolower(ch) == 'q')
    {
        pop_state();
    }
}

void game::state_game_menu(int input)
{
    switch (tolower(input))
    {
    case 'q':
        set_state(&game::state_quit_confirm);
        break;
    case 'n':
        set_state(&game::state_ask_number_of_players);
        break;
    case 'm':
        set_state(&game::state_multiplayer_menu);
        break;
    case 'c':
        if (flags_ & FLAG_CONTINUE)
        {
            flags_ &= ~(FLAG_CONTINUE);
            set_needs_clear();
            set_state(&game::state_playing);
        }
        break;
    case 's':
        break;
    }
}

void game::state_multiplayer_menu(int input)
{
    switch (tolower(input))
    {
    case 'q':
        set_state(&game::state_game_menu);
        break;
    case 'h':
        flags_ |= FLAG_HOSTING;
        set_state(&game::state_ask_name);
        break;
    case 'j':
        flags_ |= FLAG_JOINING;
        set_state(&game::state_multiplayer_join);
        break;
    }
}

void game::state_multiplayer_join(int input)
{
    switch (tolower(input))
    {
    case 'o':
        set_state(&game::state_ask_name);
        break;
    case 'l':
        flags_ |= FLAG_LAN;

        set_state(&game::state_multiplayer_join_game);
        break;
    }
}

void game::state_multiplayer_join_game(int input)
{
    if (input == CACA_KEY_RETURN || input == 10)
    {

        string host = get_buffer();
        string port;
        if (host.empty()) {
            display_alert(2000, "You must provide a host to connect to!");
            return;
        }

        auto pos = host.find(':');

        if (pos == std::string::npos) {
            display_alert(2000, "You must include a port in the address.");
            return;
        }

        port = host.substr(pos + 1);

        host = host.substr(0, pos);

        settings_.set_string("lan_host", host);
        settings_.set_string("lan_port", port);

        clear_buffer();

        set_state(&game::state_ask_name);
    }
    else if (input == 8 || input == 127)
    {
        pop_from_buffer();
    }
    else if (isprint(input))
    {
        add_to_buffer(input);
    }

    set_needs_display();
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
    }
}

void game::state_waiting_for_connections(int input)
{
    switch (tolower(input))
    {
    case 'q':
        set_state(&game::state_multiplayer_menu);
        break;
    case 's':
        if (players_.size() > 1)
        {
            set_state(&game::state_playing);

            matchmaker_.notify_game_start();

            set_needs_display();

            set_needs_clear();
        }
        break;
    }
}

void game::state_client_waiting_to_start(int input)
{

}

void game::state_quit_confirm(int input)
{
    if (tolower(input) != 'n')
    {
        action_disconnect();

        on_quit();
    }
    else
    {
        pop_alert();

        set_needs_display();
    }
}

void game::state_playing(int input)
{

    //check ascii commands in lower case
    switch (tolower(input))
    {
    case 'r':
        if (flags_ & FLAG_WAITING_FOR_TURN)
        {
            display_alert(2000, "It is not your turn yet.");
            break;
        }
        action_roll_dice();
        break;
    case '?':
        set_state(&game::state_help_menu);
        break;
    case 'q':
        set_state(&game::state_quit_confirm);
        break;
    default:
        break;

    }
}

void game::state_rolling_dice(int input)
{
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
    case 'q':
        set_state(&game::state_playing);
        break;
    case '?':
        set_state(&game::state_help_menu);
        break;
    case 'r':
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

