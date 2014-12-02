#include "game.h"
#include "player.h"
#include <cstring>
#include <arg3/str_util.h>
#include "log.h"

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
}

game::state_handler game::state() const
{
    return state_;
}

void game::recover_state()
{
    state_ = lastState_;

    clear_alerts();

    clear_events();
}

void game::set_state(state_handler value)
{
    lastState_ = state_;

    state_ = value;

    clear_alerts();

    clear_events();
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

bool game::alive() const
{
    return state_ != nullptr;
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

void game::display_alert(const vector<string> &messages)
{
    display_alert([&](const alert_box & a)
    {
        const int ymod = messages.size() / 2;

        int pos = 0;

        for (const auto &msg : messages)
        {
            if ( pos < ymod)
                put(a.center_x() - (msg.length() / 2), a.center_y() - (ymod - pos), msg.c_str());
            else
                put(a.center_x() - (msg.length() / 2), a.center_y() + (pos - ymod), msg.c_str());

            pos++;
        }
    });
}

void game::display_alert(int millis, const vector<string> &messages)
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

void game::for_players(std::function<void(const shared_ptr<player> &p)> funk)
{
    for (const auto &p : players_)
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

shared_ptr<player> game::this_player() const
{
    if (players_.size() == 0) return nullptr;

    return players_[0];
}

shared_ptr<player> game::find_player_by_id(const string &id) const
{
    for (const auto &player : players_)
    {
        if (player->id() == id) return player;
    }

    return nullptr;
}

