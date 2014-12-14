#include "game.h"
#include "player.h"
#include "log.h"
#include <cstring>
#include <arg3/str_util.h>

using namespace arg3;

using namespace std::placeholders;

const game::game_state game::state_table[] =
{
    { &game::finish_menu, &game::state_playing, &game::display_player_scores, NULL},
    { &game::display_game_menu, &game::state_game_menu, NULL, &game::exit_game},
    { &game::display_ask_name, &game::state_ask_name, NULL, NULL},
    { &game::display_dice_roll, &game::state_rolling_dice, NULL, NULL},
    { &game::display_confirm_quit, &game::state_quit_confirm, NULL, NULL},
    { &game::display_help, &game::state_help_menu, NULL, NULL},
    { &game::display_ask_number_of_players, &game::state_ask_number_of_players, NULL, NULL},
    { &game::display_multiplayer_menu, &game::state_multiplayer_menu, NULL, NULL},
    { &game::display_waiting_for_connections, &game::state_waiting_for_connections, NULL, &game::exit_multiplayer},
    {NULL, NULL, NULL, NULL}
};

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

void game::pop_state()
{
    if (!states_.empty())
    {
        auto state = states_.top();

        states_.pop();

        if (state && state->on_exit)
            bind(state->on_exit, this)();

        if (!states_.empty())
        {
            state = states_.top();

            if (state && state->on_init)
                bind(state->on_init, this)();

            set_needs_display();
        }
    }
}

const game::game_state *game::find_state(state_handler value)
{
    for (size_t i = 0; state_table[i].on_execute != NULL; i++)
    {
        if (state_table[i].on_execute == value)
            return &state_table[i];
    }
    return nullptr;
}

void game::set_state(state_handler value)
{
    clear_alerts();

    clear_events();

    auto state = find_state(value);

    if (state)
    {
        if (state->on_init)
        {
            bind(state->on_init, this)();

            set_needs_display();
        }

        states_.push(state);
    }
}

bool game::is_state(state_handler value)
{
    return !states_.empty() && states_.top()->on_execute == value;
}

void game::on_start()
{
    set_state(&game::state_game_menu);
}

bool game::alive() const
{
    return !states_.empty();
}

bool game::is_online() const
{
    return flags_ & (FLAG_HOSTING | FLAG_JOINING);
}

void game::on_display()
{
    if (!states_.empty())
    {
        auto state = states_.top();

        if (state && state->on_display)
            bind(state->on_display, this)();
    }
}

void game::on_resize(int width, int height)
{
}

void game::on_quit()
{
    while (!states_.empty())
        states_.pop();
}

void game::on_key_press(int input)
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
        set_needs_display();
        set_needs_clear();
        return;
    }
    case CACA_KEY_DOWN:
    {
        int mode = displayMode_;
        if (mode == HORIZONTAL)
            displayMode_ = MINIMAL;
        else
            displayMode_ = static_cast<display_mode>(--mode);
        set_needs_display();
        set_needs_clear();
        return;
    }
    case CACA_KEY_LEFT:
    case CACA_KEY_RIGHT:
        if (displayMode_ == MINIMAL)
        {
            minimalLower_ = !minimalLower_;
            set_needs_display();
            set_needs_clear();
        }
        return;

    case CACA_KEY_ESCAPE:
        pop_state();
        return;
    }

    if (!states_.empty())
    {
        auto state = states_.top();

        if (state && state->on_execute)
            bind(state->on_execute, this, _1)(input);
    }
}

bool game::is_playing()
{
    return is_state(&game::state_playing);
}

void game::exit_multiplayer()
{
    flags_ &= ~(FLAG_HOSTING | FLAG_JOINING);

    matchmaker_.stop();
}

void game::exit_game()
{
    set_state(&game::state_quit_confirm);
}

void game::finish_menu()
{
    while (!states_.empty())
    {
        auto state = states_.top();

        if (state && state->on_execute == &game::state_game_menu)
        {
            break;
        }

        states_.pop();
    }
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

void game::display_alert(const function<void(const alert_box &a)> funk)
{
    static alert_dimensions dimensions(this);

    caca_game::display_alert(&dimensions, funk);
}

void game::display_alert(int millis, const function<void(const alert_box &)> funk, const function<void()> pop)
{
    display_alert(funk);

    pop_alert(millis, pop);
}

void game::display_alert(const string &message, const function<void(const alert_box &a)> funk)
{
    display_alert([ message, funk](const alert_box & a)
    {
        a.center(message);

        if (funk != nullptr)
            funk(a);
    });
}

void game::display_alert(int millis, const string &message, const function<void(const alert_box &a)> funk, const function<void()> pop)
{
    display_alert(message, funk);

    pop_alert(millis, pop);
}

void game::display_alert(const vector<string> &messages, const std::function<void(const alert_box &a)> funk)
{
    display_alert([ &, messages, funk](const alert_box & a)
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

        if (funk != nullptr)
            funk(a);
    });
}

void game::display_alert(int millis, const vector<string> &messages, const function<void(const alert_box &a)> funk, const function<void()> pop)
{
    display_alert(messages, funk);

    pop_alert(millis, pop);
}

game::alert_dimensions::alert_dimensions(game *game) : game_(game) {}

int game::alert_dimensions::x() const
{
    switch (game_->displayMode_)
    {
    default:
        return 8;
    case HORIZONTAL:
        return 48;
    }
}

int game::alert_dimensions::y() const
{
    switch (game_->displayMode_)
    {
    case VERTICAL:
        return 20;
    default:
    case MINIMAL:
    case HORIZONTAL: return 8;
    }
}

int game::alert_dimensions::w() const
{
    return 60;
}
int game::alert_dimensions::h() const
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

    set_needs_display();
}

void game::next_player()
{
    if (currentPlayer_ < players_.size())
    {
        auto player = players_[currentPlayer_];
        player->reset();
        player_engine.reset();
    }

    if (++currentPlayer_ >= players_.size())
        currentPlayer_ = 0;

    if (is_online())
    {
        if ( currentPlayer_ != 0)
            flags_ |= FLAG_WAITING_FOR_TURN;
        else
            flags_ &= ~FLAG_WAITING_FOR_TURN;
    }
    set_needs_display();
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

    if (is_online())
    {
        if ( currentPlayer_ != 0)
            flags_ |= FLAG_WAITING_FOR_TURN;
        else
            flags_ &= ~FLAG_WAITING_FOR_TURN;
    }
    set_needs_display();
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

