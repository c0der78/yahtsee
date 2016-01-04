#include "caca_game.h"
#include <iostream>
#include <algorithm>
#include "log.h"

game_event::game_event(const function<void()> callback) : millis_(0), callback_(callback), ready_(true)
{
}

/* Game events */
game_event::game_event(unsigned millis, const function<void()> callback) : millis_(millis), callback_(callback), ready_(false)
{
    worker_ = std::thread(&game_event::wait, this);
}

game_event::game_event(game_event &&other)
    : millis_(other.millis_), callback_(std::move(other.callback_)), ready_(other.ready_), worker_(std::move(other.worker_))
{
}

game_event::~game_event()
{
    if (worker_.joinable()) {
        worker_.join();
    }
}

game_event &game_event::operator=(game_event &&other)
{
    millis_ = other.millis_;
    callback_ = std::move(other.callback_);
    ready_ = other.ready_;
    worker_ = std::move(other.worker_);
    return *this;
}

unsigned game_event::millis() const
{
    return millis_;
}

bool game_event::ready() const
{
    return ready_;
}
void game_event::perform() const
{
    if (callback_ != nullptr) {
        callback_();
    }
}

void game_event::clear()
{
    callback_ = nullptr;
    ready_ = true;
}

void game_event::wait()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(millis_));
    ready_ = true;
}

/* Caca game */

caca_game::caca_game() : canvas_(NULL), display_(NULL), frame_(1)
{
}

caca_game::~caca_game()
{
    reset();
}

//! resets the game display
void caca_game::reset()
{
    unique_lock<recursive_mutex> lock(mutex_);

    frame_ = 1;

    clear_alerts();

    clear_all_events();

    if (canvas_ != NULL) {
        caca_free_canvas(canvas_);
        canvas_ = NULL;
    }
    if (display_ != NULL) {
        caca_free_display(display_);
        display_ = NULL;
    }
}

//! starts the game display
void caca_game::start()
{
    reset();

    unique_lock<recursive_mutex> lock(mutex_);

    canvas_ = caca_create_canvas(0, 0);

    display_ = caca_create_display(canvas_);

    if (display_ == NULL) {
        throw runtime_error("Failed to create display");
    }

    init_canvas(canvas_);

    caca_set_frame(canvas_, frame_);

    on_start();

    set_needs_display();
}

//! updates the game input, events and display
void caca_game::update()
{
    unique_lock<recursive_mutex> lock(mutex_);

    if (display_ == NULL) {
        return;
    }

    update_input();

    update_events();

    update_display();
}

//! updates the game input
void caca_game::update_input()
{
    // grab a lock
    unique_lock<recursive_mutex> lock(mutex_);

    // query some caca events
    if (caca_get_event(display_, CACA_EVENT_QUIT | CACA_EVENT_RESIZE | CACA_EVENT_KEY_RELEASE, &event_, 1) != 0) {
        // handle quitting
        if (caca_get_event_type(&event_) & CACA_EVENT_QUIT) {
            on_quit();
            return;
        }

        // handle resizing
        if (caca_get_event_type(&event_) & CACA_EVENT_RESIZE) {
            int height = caca_get_event_resize_height(&event_);
            int width = caca_get_event_resize_width(&event_);

            on_resize(width, height);

            set_needs_display();
        }

        // handle key input
        if (caca_get_event_type(&event_) & CACA_EVENT_KEY_RELEASE) {
            int input = caca_get_event_key_ch(&event_);

            on_key_press(input);

            set_needs_display();
        }
    }
}

//! update the game events
void caca_game::update_events()
{
    // grab an event lock
    unique_lock<recursive_mutex> lock(eventsMutex_);

    /* use remove-erase idiom on events that are finished */
    events_.erase(remove_if(events_.begin(), events_.end(), [&](const game_event &ev) {
                      if (ev.ready()) {
                          ev.perform();
                          set_needs_clear();
                          return true;
                      }
                      return false;
                  }), events_.end());
}

//! update the game display
void caca_game::update_display()
{
    // grab a lock
    unique_lock<recursive_mutex> lock(mutex_);

    // clea the display if needed
    if (flags_ & FLAG_NEEDS_CLEAR) {
        clear_display();

        flags_ &= ~FLAG_NEEDS_CLEAR;
    }

    // call display handler if needed
    if (flags_ & FLAG_NEEDS_DISPLAY) {
        on_display();

        // if there is an alert box, display it as well
        if (!alert_boxes_.empty()) {
            alert_boxes_.top().display();
        }

        caca_refresh_display(display_);

        flags_ &= ~FLAG_NEEDS_DISPLAY;
    }
}

//! will clear the caca canvas and reset the cursor
void caca_game::clear_display()
{
    unique_lock<recursive_mutex> lock(mutex_);

    caca_clear_canvas(canvas_);

    init_canvas(canvas_);

    set_cursor(0, 0);

    set_needs_display();
}

//! flag the game needs to update its display
void caca_game::set_needs_display()
{
    flags_ |= FLAG_NEEDS_DISPLAY;
}

//! flag the game needs to clear its display
void caca_game::set_needs_clear()
{
    flags_ |= FLAG_NEEDS_CLEAR;
}

//! clear any pending alerts in the game
void caca_game::clear_alerts()
{
    // grab an alert lock
    unique_lock<recursive_mutex> lock(alertsMutex_);

    // check if nothing to do
    if (alert_boxes_.empty()) {
        return;
    }

    // pop everything
    while (!alert_boxes_.empty()) {
        alert_boxes_.pop();
    }

    // set display flags
    set_needs_display();

    set_needs_clear();
}

//! clear pending events in the game
void caca_game::clear_all_events()
{
    // grab an event lock
    unique_lock<recursive_mutex> lock(eventsMutex_);

    // check if nothing to do
    if (events_.empty()) {
        return;
    }

    // clear everything
    for (auto &e : events_) {
        e.clear();
    }
}
void caca_game::clear_timed_events()
{
    // grab an event lock
    unique_lock<recursive_mutex> lock(eventsMutex_);

    // check if nothing to do
    if (events_.empty()) {
        return;
    }

    // clear everything
    for (auto &e : events_) {
        if (e.millis() > 0) {
            e.clear();
        }
    }
}
// set the cursor position
void caca_game::set_cursor(int x, int y)
{
    unique_lock<recursive_mutex> lock(mutex_);

    caca_gotoxy(canvas_, x, y);
}

int caca_game::get_cursor_x() const
{
    return caca_wherex(canvas_);
}

int caca_game::get_cursor_y() const
{
    return caca_wherey(canvas_);
}

void caca_game::set_color(int fg)
{
    unique_lock<recursive_mutex> lock(mutex_);

    caca_set_color_ansi(canvas_, fg, CACA_TRANSPARENT);
}

// put a color code at a position
void caca_game::put_color(int x, int y, int fg)
{
    unique_lock<recursive_mutex> lock(mutex_);

    uint32_t attr = ((uint32_t)(CACA_TRANSPARENT | 0x40) << 18) | ((uint32_t)(fg | 0x40) << 4);

    caca_put_attr(canvas_, x, y, attr);
}

// put a string at a position
void caca_game::put(int x, int y, const char *value)
{
    if (value && *value) {
        // grab a lock
        unique_lock<recursive_mutex> lock(mutex_);

        // put the string
        caca_put_str(canvas_, x, y, value);

        // flag the display
        set_needs_display();
    }
}

void caca_game::put(int x, int y, int value)
{
    unique_lock<recursive_mutex> lock(mutex_);

    caca_put_char(canvas_, x, y, value);

    set_needs_display();
}

void caca_game::display_alert(dimensional *dimensions, const function<void(const alert_box &)> callback)
{
    unique_lock<recursive_mutex> alert_lock(alertsMutex_);

    alert_boxes_.push(std::move(alert_box(this, dimensions, callback)));

    alert_boxes_.top().display();

    unique_lock<recursive_mutex> display_lock(mutex_);

    caca_refresh_display(display_);
}

const alert_box &caca_game::displayed_alert() const
{
    return alert_boxes_.top();
}

bool caca_game::has_alert() const
{
    return alert_boxes_.size() > 0;
}

void caca_game::pop_alert()
{
    if (!alert_boxes_.empty()) {
        unique_lock<recursive_mutex> lock(alertsMutex_);

        alert_boxes_.pop();

        set_needs_display();

        set_needs_clear();
    }
}

void caca_game::pop_alert(int millis, const std::function<void()> funk)
{
    add_event(millis, [&, funk]() {
        pop_alert();

        if (funk != nullptr) {
            funk();
        }
    });
}

void caca_game::new_frame()
{
    unique_lock<recursive_mutex> lock(mutex_);

    caca_create_frame(canvas_, ++frame_);

    set_needs_display();
}

void caca_game::pop_frame()
{
    unique_lock<recursive_mutex> lock(mutex_);
    caca_set_frame(canvas_, --frame_);

    set_needs_display();
}

int caca_game::frames() const
{
    return frame_;
}

size_t caca_game::add_to_buffer(int ch)
{
    buf_.put(ch);

    return buf_.str().length();
}

void caca_game::clear_buffer()
{
    buf_.str("");
}

void caca_game::pop_from_buffer()
{
    string str = buf_.str();

    str.pop_back();

    buf_.str(str);
}

string caca_game::get_buffer()
{
    return buf_.str();
}

void caca_game::add_event(unsigned millis, const function<void()> callback)
{
    unique_lock<recursive_mutex> lock(eventsMutex_);

    events_.emplace_back(millis, callback);
}
void caca_game::add_event(const function<void()> callback)
{
    unique_lock<recursive_mutex> lock(eventsMutex_);

    events_.emplace_back(callback);
}
