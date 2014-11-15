#include "caca_game.h"
#include <iostream>

game_event::game_event(unsigned millis, function<void()> callback) : millis_(millis), callback_(callback), ready_(false)
{
    worker_ = std::thread(&game_event::wait, this);
}

game_event::game_event(game_event &&other) : millis_(other.millis_), callback_(std::move(other.callback_)), ready_(other.ready_), worker_(std::move(other.worker_))
{

}

game_event::~game_event()
{
    if (worker_.joinable())
        worker_.join();
}

game_event &game_event::operator=(game_event && other)
{
    millis_ = other.millis_;
    callback_ = std::move(other.callback_);
    ready_ = other.ready_;
    worker_ = std::move(other.worker_);
    return *this;
}

bool game_event::ready() const
{
    return ready_;
}
void game_event::perform() const
{
    callback_();
}

void game_event::wait()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(millis_));
    ready_ = true;
}

caca_game::caca_game() : frame_(1), canvas_(NULL), display_(NULL)
{
}

caca_game::~caca_game()
{
    reset();
}

void caca_game::reset()
{
    frame_ = 1;

    if (canvas_ != NULL)
    {
        caca_free_canvas(canvas_);
        canvas_ = NULL;
    }
    if (display_ != NULL)
    {
        caca_free_display(display_);
        display_ = NULL;
    }
}

void caca_game::start()
{
    reset();

    canvas_ = caca_create_canvas(0, 0);

    display_ = caca_create_display(canvas_);

    if (display_ == NULL)
    {
        cerr << "Failed to create display" << endl;
        exit(1);
    }

    init_canvas(canvas_);

    caca_set_frame(canvas_, frame_);

    on_start();

    caca_refresh_display(display_);
}

void caca_game::update()
{
    if (display_ == NULL) return;

    if (caca_get_event(display_, CACA_EVENT_QUIT | CACA_EVENT_RESIZE | CACA_EVENT_KEY_RELEASE, &event_, 0) != 0)
    {
        if (caca_get_event_type(&event_) & CACA_EVENT_QUIT)
        {
            on_quit();
            return;
        }

        if (caca_get_event_type(&event_) & CACA_EVENT_RESIZE)
        {
            int height = caca_get_event_resize_height(&event_);
            int width = caca_get_event_resize_width(&event_);

            on_resize(width, height);

            caca_refresh_display(display_);
        }

        if (caca_get_event_type(&event_) & CACA_EVENT_KEY_RELEASE)
        {
            int input = caca_get_event_key_ch(&event_);

            on_key_press(input);
        }
    }

    timed_events_.erase(remove_if(timed_events_.begin(), timed_events_.end(), [](const game_event & ev)
    {
        if (ev.ready())
        {
            ev.perform();
            return true;
        }
        return false;
    }), timed_events_.end());
}

void caca_game::refresh(bool reset)
{
    if (reset)
    {
        clear();
    }

    refresh_display(reset);

    caca_refresh_display(display_);
}

void caca_game::clear()
{
    caca_clear_canvas(canvas_);

    set_cursor(0, 0);

    init_canvas(canvas_);

    while (!alert_boxes_.empty())
        alert_boxes_.pop();
}

void caca_game::set_cursor(int x, int y)
{
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

void caca_game::put(int x, int y, const char *value)
{
    if (value && *value)
        caca_put_str(canvas_, x, y, value);
}

void caca_game::put(int x, int y, int value)
{
    caca_put_char(canvas_, x, y, value);
}

void caca_game::display_alert(int x, int y, int width, int height, function<void(const alert_box &)> callback)
{
    alert_boxes_.emplace(canvas_, display_, x, y, width, height, callback);

    alert_boxes_.top().display();
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
    if (!alert_boxes_.empty())
        alert_boxes_.pop();

    if (!alert_boxes_.empty())
    {
        alert_boxes_.top().display();
    }
    caca_refresh_display(display_);
}

void caca_game::pop_alert(int millis, std::function<void()> funk)
{
    add_event(millis, [&]()
    {
        pop_alert();
    });

    if (funk != nullptr)
        add_event(millis, funk);
}

void caca_game::new_frame()
{
    caca_create_frame(canvas_, ++frame_);
}

void caca_game::pop_frame()
{
    caca_set_frame(canvas_, --frame_);
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

string caca_game::get_buffer()
{
    return buf_.str();
}

void caca_game::add_event(unsigned millis, function<void()> callback)
{
    timed_events_.emplace_back(millis, callback);
}
