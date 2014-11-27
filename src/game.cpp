#include "game.h"
#include "player.h"
#include <cstring>
#include <arg3/str_util.h>

using namespace arg3;

using namespace std::placeholders;

game::game() : upperbuf_(NULL), lowerbuf_(NULL), menubuf_(NULL), headerbuf_(NULL), helpbuf_(NULL), upperbufSize_(0),
    lowerbufSize_(0), menubufSize_(0), headerbufSize_(0), helpbufSize_(0), displayMode_(MINIMAL), numPlayers_(0),
    matchmaker_(this), flags_(0), currentPlayer_(0)
{
}

void game::reset()
{
    caca_game::reset();

    if (upperbuf_ != NULL)
    {
        free(upperbuf_);
        upperbuf_ = NULL;
        upperbufSize_ = 0;
    }

    if (lowerbuf_ != NULL)
    {
        free(lowerbuf_);
        lowerbuf_ = NULL;
        lowerbufSize_ = 0;
    }

    if (menubuf_ != NULL)
    {
        free(menubuf_);
        menubuf_ = NULL;
        menubufSize_ = 0;
    }

    if (helpbuf_ != NULL)
    {
        free(helpbuf_);
        helpbuf_ = NULL;
        helpbufSize_ = 0;
    }

    if (headerbuf_ != NULL)
    {
        free(headerbuf_);
        headerbuf_ = NULL;
        headerbufSize_ = 0;
    }

    matchmaker_.stop();
}

game::state_handler game::state() const
{
    return state_;
}

void game::recover_state()
{
    state_ = lastState_;
}

void game::set_state(state_handler value)
{
    lastState_ = state_;

    state_ = value;
}

bool game::is_state(state_handler value)
{
    return state_ == value;
}

void game::on_start()
{
    set_state(&game::state_game_menu);

    display_game_menu();
}

void game::display_game_menu()
{
    display_alert([&](const alert_box & a)
    {
        caca_import_area_from_memory(a.canvas(), a.x() + 4, a.y() + 3, menubuf_, menubufSize_, "caca");
    });
}

void game::display_ask_name()
{
    display_alert([&](const alert_box & a)
    {
        char buf[BUFSIZ + 1] = {0};

        snprintf(buf, BUFSIZ, "What is Player %zu's name?", players_.size() + 1);

        int mod = (strlen(buf) / 2);

        put(a.center_x() - mod, a.center_y() - 1, buf);
        set_cursor(a.center_x() - mod, a.center_y());
    });
}

void game::display_ask_number_of_players()
{
    display_alert("How many players?");
}

bool game::alive() const
{
    return state_ != nullptr;
}

void game::display_multiplayer_menu()
{
    display_alert([&](const alert_box & a)
    {
        string buf1 = "'h' : host a game";
        string buf2 = "'j' : join a game";

        int xmod = min(buf1.length() / 2, buf2.length() / 2);

        put(a.center_x() - xmod, a.center_y() - 1, buf1.c_str());
        put(a.center_x() - xmod, a.center_y(), buf2.c_str());
    });
}

void game::action_host_game()
{
    display_alert("Starting server...");

    string error;

    bool response = matchmaker_.host(&error);

    pop_alert(); // done registration

    if (!response)
    {
        display_alert(2000, { "Unable to register game at this time.", error } );

        return;
    }

    set_state(&game::state_waiting_for_connections);

    display_alert("Waiting for connections...");

    // TODO: add server listener to display connections
}

void game::action_join_game()
{
    // TODO: list available games.  maybe based on ip locations

    display_alert("Finding game to join...");

    string error;

    bool result = matchmaker_.join_best_game(&error);

    pop_alert();

    if (!result)
    {
        display_alert(2000, {"Unable to find game to join at this time.", error});
        return;
    }
}

void game::action_joined_game()
{
    char buf[BUFSIZ + 1] = {0};

    pop_alert();

    vector<string> message;

    int count = 1;

    for (const auto & p : players_)
    {
        snprintf(buf, BUFSIZ, "%2d: %s", count++, p->name().c_str());
        message.push_back(buf);
    }

    message.push_back(" ");

    message.push_back("Waiting for host to start game...");

    display_alert(message);
}

void game::action_disconnected()
{
    if(!alive()) return;

    clear_alerts();

    display_alert(2000, "Disconnected from server.");

    set_state(&game::state_game_menu);

    display_game_menu();
}

void game::action_add_network_player(const shared_ptr<player> &player)
{
    char buf[BUFSIZ + 1] = {0};

    players_.push_back(player);

    matchmaker_.notify_player_joined(player);

    pop_alert();

    vector<string> message;

    int count = 1;

    for (const auto & p : players_)
    {
        snprintf(buf, BUFSIZ, "%2d: %s", count++, p->name().c_str());
        message.push_back(buf);
    }

    message.push_back(" ");

    message.push_back("Waiting for more players, press 's' to start the game.");

    display_alert(message);
}

void game::action_remove_network_player(connection *c)
{
    auto it = find_if(players_.begin(), players_.end(), [&c](const shared_ptr<player> &p)
    {
        return p->connect1on() == c;
    });

    if (it != players_.end())
    {
        auto p = *it;

        players_.erase(it);

        if (players_.size() == 0)
        {
            players_.clear();

            set_state(&game::state_game_menu);

            display_game_menu();

            clear_alerts();

        }
        display_alert(2000, p->name() + " has left.");

        matchmaker_.notify_player_left(p);

    }
}

void game::refresh_display(bool reset)
{
    shared_ptr<player> player = current_player();

    if (player != nullptr && is_playing())
    {
        if (reset)
            player->reset();

        int x = 46;

        put(50, 2, player->name().c_str());

        switch (displayMode_)
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
            yaht::scoresheet::value_type lower_score_total = display_upper_scores(player->score(), x , 9 );
            display_lower_scores(player->score(), lower_score_total, 46, 28);
            put(0, 51, HELP);
            break;
        }
        case HORIZONTAL:
        {
            yaht::scoresheet::value_type lower_score_total = display_upper_scores(player->score(), x , 9 );
            display_lower_scores(player->score(), lower_score_total, 122, 2);
            put(76, 25, HELP);
            break;
        }
        }

    }

}

void game::display_already_scored()
{
    display_alert(2000, "You've already scored that.");
}

void game::display_dice(shared_ptr<yaht::player> player, int x, int y)
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

void game::display_dice_roll()
{
    display_alert([&](const alert_box & box)
    {
        display_dice(current_player(), box.x(), box.y());

    });
}

void game::action_roll_dice()
{
    auto player = current_player();

    if (player->roll_count() < 3)
    {
        player->roll();

        display_dice_roll();
    }
    else
    {
        display_alert(2000, "You must choose a score after three rolls.");
    }
}

void game::action_finish_turn()
{
    set_state(&game::state_playing);

    next_player();

    refresh(true);
}

void game::action_select_die(shared_ptr<yaht::player> player, int d)
{
    if (player->is_kept(d))
        player->keep_die(d, false);
    else
        player->keep_die(d, true);

    auto box = displayed_alert();

    box.display();
}


void game::action_lower_score(shared_ptr<yaht::player> player, yaht::scoresheet::type type)
{
    if (!player->score().lower_score(type))
    {
        player->score().lower_score(type, player->calculate_lower_score(type));

        action_finish_turn();
    }
    else
    {
        display_already_scored();
    }
}

void game::action_score(shared_ptr<yaht::player> player, int n)
{
    if (!player->score().upper_score(n))
    {
        player->score().upper_score(n, player->calculate_upper_score(n));

        action_finish_turn();
    }
    else
    {
        display_already_scored();
    }
}

void game::action_score_best(shared_ptr<yaht::player> player)
{
    auto best_upper = player->calculate_best_upper_score();

    auto best_lower = player->calculate_best_lower_score();

    if (best_upper.second > best_lower.second)
    {

        if (!player->score().upper_score(best_upper.first))
        {
            player->score().upper_score(best_upper.first, best_upper.second);

            action_finish_turn();
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

            action_finish_turn();
        }
        else
        {
            display_already_scored();
        }
    }
    else
    {
        display_alert(2000, "No best score found!");
    }
}


void game::display_confirm_quit()
{
    display_alert("Are you sure you want to quit? (Y/n)");
}

void game::on_resize(int width, int height)
{
}

void game::on_quit()
{
    state_ = nullptr;
}

void game::on_key_press(int input)
{
    bind(state_, this, _1)(input);
}

bool game::is_playing()
{
    return is_state(&game::state_playing);
}

void game::init_canvas(caca_canvas_t *canvas)
{
    caca_canvas_t *temp = caca_create_canvas(0, 0);

    caca_import_canvas_from_file(temp, "upper.txt", "utf8");

    upperbuf_ = caca_export_canvas_to_memory(temp, "caca", &upperbufSize_);

    caca_free_canvas(temp);

    temp = caca_create_canvas(0, 0);

    caca_import_canvas_from_file(temp, "lower.txt", "utf8");

    lowerbuf_ = caca_export_canvas_to_memory(temp, "caca", &lowerbufSize_);

    caca_free_canvas(temp);

    temp = caca_create_canvas(0, 0);

    caca_import_canvas_from_file(temp, "menu.txt", "utf8");

    menubuf_ = caca_export_canvas_to_memory(temp, "caca", &menubufSize_);

    caca_free_canvas(temp);

    temp = caca_create_canvas(0, 0);

    caca_import_canvas_from_file(temp, "help.txt", "utf8");

    helpbuf_ = caca_export_canvas_to_memory(temp, "caca", &helpbufSize_);

    caca_free_canvas(temp);

    temp = caca_create_canvas(0, 0);

    caca_import_canvas_from_file(temp, "header.txt", "utf8");

    headerbuf_ = caca_export_canvas_to_memory(temp, "caca", &headerbufSize_);

    caca_free_canvas(temp);

    switch (displayMode_)
    {
    case VERTICAL:
        caca_import_area_from_memory(canvas, 0, 0, headerbuf_, headerbufSize_, "caca");
        caca_import_area_from_memory(canvas, 0, 8, upperbuf_, upperbufSize_, "caca");
        caca_import_area_from_memory(canvas, 0, 27, lowerbuf_, lowerbufSize_, "caca");
        break;
    case HORIZONTAL:
        caca_import_area_from_memory(canvas, 0, 0, headerbuf_, headerbufSize_, "caca");
        caca_import_area_from_memory(canvas, 0, 8, upperbuf_, upperbufSize_, "caca");
        caca_import_area_from_memory(canvas, 76, 1, lowerbuf_, lowerbufSize_, "caca");
        break;
    case MINIMAL:
        caca_import_area_from_memory(canvas, 0, 0, headerbuf_, headerbufSize_, "caca");
        caca_import_area_from_memory(canvas, 0, 8, minimalLower_ ? lowerbuf_ : upperbuf_, minimalLower_ ? lowerbufSize_ : upperbufSize_, "caca");
        break;
    }
}
void game::set_display_mode(display_mode mode)
{
    displayMode_ = mode;
}


void game::display_alert(function<void(const alert_box &a)> funk)
{
    caca_game::display_alert(get_alert_x(), get_alert_y(), get_alert_w(), get_alert_h(), funk);
}

void game::display_alert(int millis, function<void(const alert_box &)> funk)
{
    display_alert(funk);
    pop_alert(millis);
}

void game::display_alert(const string &message)
{
    display_alert([&](const alert_box & a)
    {
        a.center(message);
    });
}

void game::display_alert(int millis, const string &message)
{
    display_alert(message);

    pop_alert(millis);
}

void game::display_alert(vector<string> messages)
{
    display_alert([&](const alert_box & a)
    {
        const int ymod = messages.size() / 2;

        int pos = 0;

        for (const auto & msg : messages)
        {
            if ( pos < ymod)
                put(a.center_x() - (msg.length() / 2), a.center_y() - (ymod - pos), msg.c_str());
            else
                put(a.center_x() - (msg.length() / 2), a.center_y() + (pos - ymod), msg.c_str());

            pos++;
        }
    });
}

void game::display_alert(int millis, vector<string> messages)
{
    display_alert(messages);

    pop_alert(millis);
}

int game::get_alert_x() const
{
    switch (displayMode_)
    {
    default:
        return 8;
    case HORIZONTAL:
        return 48;
    }
}

int game::get_alert_y() const
{
    switch (displayMode_)
    {
    case VERTICAL:
        return 20;
    default:
    case MINIMAL:
    case HORIZONTAL: return 8;
    }
}

int game::get_alert_w() const
{
    return 60;
}
int game::get_alert_h() const
{
    return 15;
}

void game::display_help()
{
    display_alert([&](const alert_box & a)
    {
        caca_import_area_from_memory(a.canvas(), a.x() + 4, a.y() + 3, helpbuf_, helpbufSize_, "caca");
    });
}

void game::for_players(std::function<void(const shared_ptr<player> &p)> funk)
{
    for (const auto & p : players_)
    {
        funk(p);
    }
}

void game::add_player(const shared_ptr<player> &p)
{
    players_.push_back(p);
}

void game::next_player()
{
    if (++currentPlayer_ >= players_.size())
        currentPlayer_ = 0;
}

shared_ptr<player> game::get_player(size_t index) const
{
    if (index >= players_.size()) return nullptr;

    auto it = players_.begin() + index;

    return *it;
}

void game::set_current_player(const shared_ptr<player> &p)
{
    auto it = find(players_.begin(), players_.end(), p);

    if (it != players_.end())
        currentPlayer_ = distance(players_.begin(), it);
}

shared_ptr<player> game::current_player() const
{
    size_t pSize = players_.size();

    if (pSize == 0) return nullptr;

    return players_[currentPlayer_];
}

shared_ptr<player> game::find_player_by_id(const string &id) const
{
    for (const auto & player : players_)
    {
        if (player->id() == id) return player;
    }

    return nullptr;
}

yaht::scoresheet::value_type game::display_upper_scores(const yaht::scoresheet &score, int x, int y)
{
    yaht::scoresheet::value_type total_score = 0;

    for (int i = 0; i <= yaht::Constants::NUM_DICE; i++, y += 2)
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

void game::display_lower_scores(const yaht::scoresheet &score, yaht::scoresheet::value_type lower_score_total, int x, int y)
{
    yaht::scoresheet::value_type total_score = 0;

    for (int i = yaht::scoresheet::FIRST_TYPE; i < yaht::scoresheet::MAX_TYPE; i++)
    {
        yaht::scoresheet::type type = static_cast<yaht::scoresheet::type>(i);

        auto value = score.lower_score(type);

        put(x, y, std::to_string(value).c_str());

        total_score += value;

        switch (type)
        {
        default:
            y += 2;
            break;
        case yaht::scoresheet::STRAIGHT_SMALL:
        case yaht::scoresheet::STRAIGHT_BIG:
        case yaht::scoresheet::YACHT:
            y += 3;
            break;
        }
    }

    put(x, y, std::to_string(total_score).c_str());

    put(x, y + 2, std::to_string(lower_score_total).c_str());

    put(x, y + 4, std::to_string(total_score + lower_score_total).c_str());
}
