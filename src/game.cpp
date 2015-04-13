#include "game.h"
#include "player.h"
#include "log.h"
#include <cstring>
#include <fstream>
#include <arg3/str_util.h>
#include <libgen.h>
#include <archive.h>
#include <archive_entry.h>

using namespace arg3;

using namespace std::placeholders;

const char *HELP = "Type '?' to show command options.  Use the arrow keys to cycle views modes.";

/*! the state table for lookups and transitions */
const game::game_state game::state_table[] =
{
    { &game::init_playing, &game::state_playing, &game::display_player_scores, &game::stop_playing, 0},
    { &game::display_game_menu, &game::state_game_menu, NULL, &game::exit_game, 0},
    { &game::display_ask_name, &game::state_ask_name, NULL, NULL, FLAG_STATE_TRANSIENT},
    { &game::display_dice_roll, &game::state_rolling_dice, &game::display_player_scores, NULL, FLAG_STATE_TRANSIENT},
    { &game::display_confirm_quit, &game::state_quit_confirm, NULL, NULL, 0},
    { &game::display_help, &game::state_help_menu, NULL, NULL, FLAG_STATE_TRANSIENT},
    { &game::display_ask_number_of_players, &game::state_ask_number_of_players, NULL, NULL, FLAG_STATE_TRANSIENT},
    { &game::display_multiplayer_menu, &game::state_multiplayer_menu, NULL, NULL, FLAG_STATE_TRANSIENT},
    { &game::display_multiplayer_join, &game::state_multiplayer_join, NULL, NULL, FLAG_STATE_TRANSIENT},
    { &game::display_multiplayer_join_game, &game::state_multiplayer_join_game, NULL, NULL, FLAG_STATE_TRANSIENT},
    { &game::display_waiting_for_connections, &game::state_waiting_for_connections, NULL, &game::exit_multiplayer, FLAG_STATE_TRANSIENT},
    { &game::display_client_waiting_to_start, &game::state_client_waiting_to_start, NULL, &game::exit_multiplayer, FLAG_STATE_TRANSIENT},
    { &game::action_join_game, &game::state_joining_game, NULL, &game::exit_multiplayer, FLAG_STATE_TRANSIENT},
    { &game::action_join_online_game, &game::state_joining_online_game, NULL, &game::exit_multiplayer, FLAG_STATE_TRANSIENT},
    {NULL, NULL, NULL, NULL}
};

game::game() : displayMode_(MINIMAL), numPlayers_(0),
    matchmaker_(this), flags_(0), currentPlayer_(0)
{
    for (size_t i = 0; i < BUF_MAX; i++)
    {
        bufs[i] = NULL;
        bufSize[i] = 0;
    }
}


void game::load_settings(char *exe)
{
    std::ifstream inFile;

    const char *baseDir = dirname(exe);

    inFile.open(resource_file_name("yahtsee.json", baseDir));

    stringstream strStream;
    strStream << inFile.rdbuf();

    inFile.close();

    settings_.parse(strStream.str());

    if (!settings_.contains("basedir")) {
        settings_.set_string("basedir", baseDir);
    }

    if (settings_.contains("default_display"))
    {
        auto mode = settings_.get_string("default_display");

        if (mode == "horizontal") {
            displayMode_ = HORIZONTAL;
        }
        else if (mode == "vertical") {
            displayMode_ = VERTICAL;
        }
    }

    if (settings_.contains("arg3connect"))
    {
        json::object service = settings_.get("arg3connect");

        if (!service.contains("enabled") || service.get_bool("enabled"))
        {
            std::string appId = service.get_string("appId");
            std::string appToken = service.get_string("appToken");

            matchmaker_.set_api_keys(appId, appToken);
        }
    }
}

void game::reset()
{
    caca_game::reset();

    // cleanup the buffers
    for (size_t i = 0; i < BUF_MAX; i++)
    {
        if (bufs[i] != NULL)
        {
            free(bufs[i]);
            bufs[i] = NULL;
            bufSize[i] = 0;
        }
    }
}

void game::pop_state()
{
    lock_guard<recursive_mutex> lock(mutex_);

    // clear any alerts from the state
    clear_alerts();

    // clear any events from the state
    clear_events();

    if (!states_.empty())
    {
        // get the top state
        auto state = states_.top();

        // before popping it
        states_.pop();

        // perform any on_exit handler
        if (state && state->on_exit) {
            bind(state->on_exit, this)();
        }

        // have a state below?
        if (!states_.empty())
        {
            state = states_.top();

            // call the on_init method again
            if (state && state->on_init) {
                bind(state->on_init, this)();
            }

            set_needs_display();
        }
    }
}

const game::game_state *game::find_state(state_handler value)
{
    for (size_t i = 0; state_table[i].on_execute != NULL; i++)
    {
        if (state_table[i].on_execute == value) {
            return &state_table[i];
        }
    }
    return nullptr;
}

void game::set_state(state_handler value)
{
    lock_guard<recursive_mutex> lock(mutex_);

    // clear any alerts from the previous state
    clear_alerts();

    // clear any events from the previous state
    clear_events();

    auto state = find_state(value);

    if (state)
    {
        if (!states_.empty())
        {
            auto old = states_.top();

            // if the previous state is flagged transient
            if (old && (old->flags & FLAG_STATE_TRANSIENT))
            {
                // then pop it
                states_.pop();
            }
        }

        // initialize the new state
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

bool game::is_online_available() const
{
    if (!settings_.contains("arg3connect")) {
        return false;
    }

    json::object service = settings_.get("arg3connect");

    return service.contains("enabled") && service.get_bool("enabled");
}

void game::on_display()
{
    if (!states_.empty())
    {
        auto state = states_.top();

        if (state && state->on_display) {
            bind(state->on_display, this)();
        }
    }

    switch (displayMode_)
    {
    case MINIMAL:
        put(0, 25, HELP);
        break;
    case VERTICAL:
        put(0, 51, HELP);
        break;
    case HORIZONTAL:
        put(76, 23, HELP);
        break;
    }
}

void game::on_resize(int width, int height)
{
}

void game::on_quit()
{
    clear_states();
}

void game::on_key_press(int input)
{
    // check non ascii commands
    switch (input)
    {
    case CACA_KEY_UP:
    {
        int mode = displayMode_;
        if (mode == MINIMAL) {
            displayMode_ = HORIZONTAL;
        }
        else {
            displayMode_ = static_cast<display_mode>(++mode);
        }
        set_needs_display();
        set_needs_clear();
        return;
    }
    case CACA_KEY_DOWN:
    {
        int mode = displayMode_;
        if (mode == HORIZONTAL) {
            displayMode_ = MINIMAL;
        }
        else {
            displayMode_ = static_cast<display_mode>(--mode);
        }
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
    {
        auto state = states_.top();

        // if the state is not forced
        if (!(state->flags & FLAG_STATE_FORCE))
        {
            // pop it
            pop_state();
        }
        return;
    }
    }

    if (!states_.empty())
    {
        auto state = states_.top();

        // execute the current state
        if (state && state->on_execute) {
            bind(state->on_execute, this, _1)(input);
        }
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

    players_.clear();
}

void game::init_playing()
{
    if (current_player()->roll_count() > 0)
    {
        flags_ |= FLAG_ROLLING;

        logf("still rolling");
    }
}

void game::stop_playing()
{
    flags_ |= FLAG_CONTINUE;
}

void game::exit_game()
{
    set_state(&game::state_quit_confirm);
}

void game::clear_states()
{
    lock_guard<recursive_mutex> lock(mutex_);

    while (!states_.empty())
    {
        states_.pop();
    }
}

char *game::resource_file_name(const char *path, const char *dir)
{
    static char resourceDir[BUFSIZ + 1] = {0};
    static char bufs[3][BUFSIZ + 1];
    static int bufIndex = 0;

    ++bufIndex, bufIndex %= 3;

    char *buf = bufs[bufIndex];

    // if we don't have a resource directory yet
    if (!resourceDir[0])
    {
        // get the base directory
        if (dir == NULL) {
            dir = settings_.get_string("basedir").c_str();
        }

        // try an etc dir first
        snprintf(resourceDir, BUFSIZ, "%s/../etc/yahtsee", dir);

        if (!dir_exists(resourceDir))
        {
            // try some resource directories
            snprintf(resourceDir, BUFSIZ, "%s/resources", dir);

            if (!dir_exists(resourceDir))
            {
                snprintf(resourceDir, BUFSIZ, "%s/../resources", dir);

                if (!dir_exists(resourceDir)) {
                    resourceDir[0] = 0;
                }
            }
        }
    }

    // build the path
    if (!resourceDir[0]) {
        strncpy(buf, path, BUFSIZ);
    }
    else if (path && *path == '/') {
        snprintf(buf, BUFSIZ, "%s%s", resourceDir, path);
    }
    else {
        snprintf(buf, BUFSIZ, "%s/%s", resourceDir, path);
    }

    return buf;
}

void game::load_buf(const char *fileName, int index)
{
    struct archive *a;
    struct archive_entry *entry;
    int r;

    a = archive_read_new();

    archive_read_support_compression_gzip(a);
    archive_read_support_format_tar(a);

    r = archive_read_open_filename(a, resource_file_name("yahtsee.assets"), 10240);

    if (r != ARCHIVE_OK) {
        throw runtime_error("yahtsee assets not found");
    }

    void *temp = NULL;
    size_t tempSize = 0;

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    {
        if (suffix(archive_entry_pathname(entry), fileName))
        {
            tempSize = archive_entry_size(entry);
            temp = malloc(tempSize);
            r  = archive_read_data(a, temp, tempSize);

            if (r < 0)
            {
                throw runtime_error("unable to parse yahtsee assets");
            }
            break;
        }

    }
    archive_read_finish(a);

    if (temp == NULL) {
        throw runtime_error("unable to read yahtsee assets");
    }

    caca_canvas_t *canvas = caca_create_canvas(0, 0);

    caca_import_canvas_from_memory(canvas, temp, tempSize, "utf8");

    free(temp);

    bufs[index] = caca_export_canvas_to_memory(canvas, "caca", &bufSize[index]);

    caca_free_canvas(canvas);
}

void game::init_canvas(caca_canvas_t *canvas)
{
    // load some buffers from resources
    load_buf("upper.txt", BUF_UPPER);
    load_buf("lower.txt", BUF_LOWER);
    load_buf("help.txt", BUF_HELP);
    load_buf("lower_header_minimal.txt", BUF_LOWER_HEADER_MINIMAL);
    load_buf("lower_header.txt", BUF_LOWER_HEADER);
    load_buf("menu.txt", BUF_MENU);

    switch (displayMode_)
    {
    case VERTICAL:
        caca_import_area_from_memory(canvas, 0, 0, bufs[BUF_UPPER], bufSize[BUF_UPPER], "caca");
        caca_import_area_from_memory(canvas, 0, 25, bufs[BUF_LOWER_HEADER], bufSize[BUF_LOWER_HEADER], "caca");
        caca_import_area_from_memory(canvas, 0, 26, bufs[BUF_LOWER], bufSize[BUF_LOWER], "caca");
        break;
    case HORIZONTAL:
        caca_import_area_from_memory(canvas, 0, 0, bufs[BUF_UPPER], bufSize[BUF_UPPER], "caca");
        caca_import_area_from_memory(canvas, 76, 0, bufs[BUF_LOWER_HEADER], bufSize[BUF_LOWER_HEADER], "caca");
        caca_import_area_from_memory(canvas, 76, 1, bufs[BUF_LOWER], bufSize[BUF_LOWER], "caca");
        break;
    case MINIMAL:
    {
        int index = minimalLower_ ? BUF_LOWER : BUF_UPPER;
        if (minimalLower_) {
            caca_import_area_from_memory(canvas, 0, 0, bufs[BUF_LOWER_HEADER_MINIMAL], bufSize[BUF_LOWER_HEADER_MINIMAL], "caca");
        }
        caca_import_area_from_memory(canvas, 0, minimalLower_ ? 3 : 0, bufs[index], bufSize[index], "caca");
        break;
    }
    }

    if (displayMode_ != MINIMAL || !minimalLower_)
    {
        // colorize the title logo
        for (int y = 1; y <= 5; y++)
        {
            for (int x = 1; x <= 38; x++)
            {
                put_color(x, y, CACA_YELLOW);
            }
        }
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

        if (funk != nullptr) {
            funk(a);
        }
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
            if ( pos < ymod) {
                put(a.center_x() - (msg.length() / 2), a.center_y() - (ymod - pos), msg.c_str());
            }
            else {
                put(a.center_x() - (msg.length() / 2), a.center_y() + (pos - ymod), msg.c_str());
            }

            pos++;
        }

        if (funk != nullptr) {
            funk(a);
        }
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
    case HORIZONTAL:
        return 7;
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

void game::for_players(std::function<bool(const shared_ptr<player> &p)> funk)
{
    for (const auto &p : players_)
    {
        if (funk(p)) { break; }
    }
}

void game::add_player(const shared_ptr<player> &p)
{
    players_.push_back(p);

    set_needs_display();
}

void game::next_player()
{
    bool finished = true;

    for_players([&finished](const shared_ptr<player> &p) {
        if (!p->is_finished())
        {
            finished = false;
            return true;
        }
        return false;
    });

    if (finished)
    {
        action_game_over();
        return;
    }

    if (currentPlayer_ < players_.size())
    {
        auto player = players_[currentPlayer_];
        player->reset();
        player_engine.reset();
    }

    if (++currentPlayer_ >= players_.size())
    {
        currentPlayer_ = 0;
    }

    if (is_online())
    {
        if ( currentPlayer_ != 0) {
            flags_ |= FLAG_WAITING_FOR_TURN;
        }
        else {
            flags_ &= ~FLAG_WAITING_FOR_TURN;
        }
    }
    set_needs_display();
}

shared_ptr<player> game::get_player(size_t index) const
{
    if (index >= players_.size()) { return nullptr; }

    auto it = players_.begin() + index;

    return *it;
}

void game::set_current_player(const shared_ptr<player> &p)
{
    auto it = find(players_.begin(), players_.end(), p);

    if (it != players_.end()) {
        currentPlayer_ = distance(players_.begin(), it);
    }

    if (is_online())
    {
        if ( currentPlayer_ != 0) {
            flags_ |= FLAG_WAITING_FOR_TURN;
        }
        else {
            flags_ &= ~FLAG_WAITING_FOR_TURN;
        }
    }
    set_needs_display();
}

shared_ptr<player> game::current_player() const
{
    size_t pSize = players_.size();

    if (pSize == 0) { return nullptr; }

    return players_[currentPlayer_];
}

shared_ptr<player> game::this_player() const
{
    if (players_.size() == 0) { return nullptr; }

    return players_[0];
}

shared_ptr<player> game::find_player_by_id(const string &id) const
{
    for (const auto &player : players_)
    {
        if (player->id() == id) { return player; }
    }

    return nullptr;
}

