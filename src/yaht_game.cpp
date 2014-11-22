#include "yaht_game.h"
#include <cstring>
#include <arg3/str_util.h>

using namespace std::placeholders;

yaht_player::yaht_player(yaht_connection *conn, const string &name) : player(name), connection_(conn)
{
}

int yaht_player::id() const
{
    return connection_->raw_socket();
}

yaht_game::yaht_game() : upperbuf_(NULL), lowerbuf_(NULL), menubuf_(NULL), headerbuf_(NULL), helpbuf_(NULL), upperbuf_size_(0),
    lowerbuf_size_(0), menubuf_size_(0), headerbuf_size_(0), helpbuf_size_(0), display_mode_(MINIMAL), num_players_(0),
    matchmaker_(this), flags_(0)
{
}

void yaht_game::reset()
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

    if (helpbuf_ != NULL)
    {
        free(helpbuf_);
        helpbuf_ = NULL;
        helpbuf_size_ = 0;
    }

    if (headerbuf_ != NULL)
    {
        free(headerbuf_);
        headerbuf_ = NULL;
        headerbuf_size_ = 0;
    }

    matchmaker_.stop();
}

yaht_game::state_handler yaht_game::state() const
{
    return state_;
}

void yaht_game::recover_state()
{
    state_ = last_state_;
}

void yaht_game::set_state(state_handler value)
{
    last_state_ = state_;

    state_ = value;
}

bool yaht_game::is_state(state_handler value)
{
    return state_ == value;
}

void yaht_game::on_start()
{
    set_state(&yaht_game::state_game_menu);

    display_game_menu();
}

void yaht_game::display_game_menu()
{
    display_alert([&](const alert_box & a)
    {
        caca_import_area_from_memory(a.canvas(), a.x() + 4, a.y() + 3, menubuf_, menubuf_size_, "caca");
    });
}

void yaht_game::display_ask_name()
{
    display_alert([&](const alert_box & a)
    {
        char buf[BUFSIZ + 1] = {0};

        snprintf(buf, BUFSIZ, "What is Player %zu's name?", yaht_.number_of_players() + 1);

        int mod = (strlen(buf) / 2);

        put(a.center_x() - mod, a.center_y() - 1, buf);
        set_cursor(a.center_x() - mod, a.center_y());
    });
}

void yaht_game::display_ask_number_of_players()
{
    display_alert("How many players?");
}

bool yaht_game::alive() const
{
    return state_ != nullptr;
}

void yaht_game::display_multiplayer_menu()
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

void yaht_game::action_host_game()
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

    set_state(&yaht_game::state_waiting_for_connections);

    display_alert("Waiting for connections...");

    // TODO: add server listener to display connections
}

void yaht_game::action_join_game()
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

void yaht_game::action_joined_game()
{

    char buf[BUFSIZ + 1] = {0};

    pop_alert();

    vector<string> message;

    int count = 1;

    yaht_.for_players([&message, &buf, &count](const shared_ptr<player> &p)
    {
        snprintf(buf, BUFSIZ, "%2d: %s", count++, p->name().c_str());
        message.push_back(buf);
    });

    message.push_back(" ");

    message.push_back("Waiting for host to start game...");

    display_alert(message);
}

void yaht_game::action_add_network_player(const shared_ptr<yaht_player> &player)
{
    char buf[BUFSIZ + 1] = {0};

    yaht_.add_player(player);

    matchmaker_.notify_player_joined(player);

    pop_alert();

    vector<string> message;

    int count = 1;

    yaht_.for_players([&message, &buf, &count](const shared_ptr<arg3::yaht::player> &p)
    {
        snprintf(buf, BUFSIZ, "%2d: %s", count++, p->name().c_str());
        message.push_back(buf);
    });

    message.push_back(" ");

    message.push_back("Waiting for more players, press 's' to start the game.");

    display_alert(message);
}

void yaht_game::action_remove_network_player(const shared_ptr<yaht_player> &p)
{
    yaht_.remove_player(p);

    display_alert(2000, p->name() + " has left.");

    matchmaker_.notify_player_left(p);
}

void yaht_game::refresh_display(bool reset)
{
    shared_ptr<player> player = yaht_.current_player();

    if (player != nullptr && is_playing())
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
            scoresheet::value_type lower_score_total = display_upper_scores(player->score(), x , 9 );
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


void yaht_game::display_already_scored()
{
    display_alert(2000, "You've already scored that.");
}

void yaht_game::display_dice(shared_ptr<player> player, int x, int y)
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

void yaht_game::display_dice_roll()
{
    display_alert([&](const alert_box & box)
    {
        display_dice(yaht_.current_player(), box.x(), box.y());

    });
}

void yaht_game::action_roll_dice()
{
    auto player = yaht_.current_player();

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

void yaht_game::action_finish_turn()
{
    set_state(&yaht_game::state_playing);

    yaht_.next_player();

    refresh(true);
}

void yaht_game::action_select_die(shared_ptr<player> player, int d)
{
    if (player->is_kept(d))
        player->keep_die(d, false);
    else
        player->keep_die(d, true);

    auto box = displayed_alert();

    box.display();
}


void yaht_game::action_lower_score(shared_ptr<player> player, scoresheet::type type)
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

void yaht_game::action_score(shared_ptr<player> player, int n)
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

void yaht_game::action_score_best(shared_ptr<player> player)
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


void yaht_game::display_confirm_quit()
{
    display_alert("Are you sure you want to quit? (Y/n)");
}

void yaht_game::on_resize(int width, int height)
{
}

void yaht_game::on_quit()
{
    state_ = nullptr;
}

void yaht_game::on_key_press(int input)
{
    bind(state_, this, _1)(input);
}

bool yaht_game::is_playing()
{
    return is_state(&yaht_game::state_playing);
}

void yaht_game::init_canvas(caca_canvas_t *canvas)
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

    caca_import_canvas_from_file(temp, "help.txt", "utf8");

    helpbuf_ = caca_export_canvas_to_memory(temp, "caca", &helpbuf_size_);

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
void yaht_game::set_display_mode(display_mode mode)
{
    display_mode_ = mode;
}


void yaht_game::display_alert(function<void(const alert_box &a)> funk)
{
    caca_game::display_alert(get_alert_x(), get_alert_y(), get_alert_w(), get_alert_h(), funk);
}

void yaht_game::display_alert(int millis, function<void(const alert_box &)> funk)
{
    display_alert(funk);
    pop_alert(millis);
}

void yaht_game::display_alert(const string &message)
{
    display_alert([&](const alert_box & a)
    {
        a.center(message);
    });
}

void yaht_game::display_alert(int millis, const string &message)
{
    display_alert(message);

    pop_alert(millis);
}

void yaht_game::display_alert(vector<string> messages)
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

void yaht_game::display_alert(int millis, vector<string> messages)
{
    display_alert(messages);

    pop_alert(millis);
}

int yaht_game::get_alert_x() const
{
    switch (display_mode_)
    {
    default:
        return 8;
    case HORIZONTAL:
        return 48;
    }
}

int yaht_game::get_alert_y() const
{
    switch (display_mode_)
    {
    case VERTICAL:
        return 20;
    default:
    case MINIMAL:
    case HORIZONTAL: return 8;
    }
}

int yaht_game::get_alert_w() const
{
    return 60;
}
int yaht_game::get_alert_h() const
{
    return 15;
}

void yaht_game::display_help()
{
    display_alert([&](const alert_box & a)
    {
        caca_import_area_from_memory(a.canvas(), a.x() + 4, a.y() + 3, helpbuf_, helpbuf_size_, "caca");
    });
}

shared_ptr<yaht_player> yaht_game::find_player_by_id(int id) const
{
    for (size_t i = 0; i < yaht_.number_of_players(); i++)
    {
        auto player = dynamic_pointer_cast<yaht_player>(yaht_.get_player(i));

        if (player->id() == id) return player;
    }

    return nullptr;
}

scoresheet::value_type yaht_game::display_upper_scores(const scoresheet &score, int x, int y)
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

void yaht_game::display_lower_scores(const scoresheet &score, scoresheet::value_type lower_score_total, int x, int y)
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
