#ifndef _CACA_GAME_H_
#define _CACA_GAME_H_

#include <caca.h>
#include <vector>
#include <stack>
#include <thread>
#include <sstream>
#include <mutex>
#include "alert_box.h"

using namespace std;

class game_event
{
public:
    game_event(unsigned millis, const function<void()> callback);
    game_event(const game_event &) = delete;
    game_event(game_event  &&e);
    ~game_event();
    game_event &operator=(const game_event &) = delete;
    game_event &operator=(game_event && );
    bool ready() const;
    void perform() const;
    void clear();
private:
    void wait();
    unsigned millis_;
    function<void()> callback_;
    bool ready_;
    thread worker_;
};

class caca_game
{
public:
    caca_game();

    virtual ~caca_game();

    virtual void reset();

    void start();

    void update();

    virtual void on_quit() = 0;

    virtual void on_resize(int width, int height) = 0;

    virtual void on_key_press(int input) = 0;

    virtual void on_start() = 0;

    virtual void on_display() = 0;

    void update_display();

    void update_input();

    void update_events();

    void clear_display();

    void set_cursor(int x, int y);

    int get_cursor_x() const;

    int get_cursor_y() const;

    void set_color(int fg);

    void put_color(int x, int y, int fg);

    void put(int x, int y, const char *value);

    void put(int x, int y, int value);

    void display_alert(dimensional *dimensions, const function<void(const alert_box &)> callback);

    const alert_box &displayed_alert() const;

    bool has_alert() const;

    void pop_alert();

    void pop_alert(int millis, const std::function<void()> funk = nullptr);

    void clear_alerts();

    void clear_events();

    void new_frame();

    void pop_frame();

    int frames() const;

    size_t add_to_buffer(int ch);

    void clear_buffer();

    void pop_from_buffer();

    string get_buffer();

    void add_event(unsigned millis, const function<void()> callback);

    void set_needs_display();

    void set_needs_clear();
protected:

    virtual void init_canvas(caca_canvas_t *canvas) = 0;

    caca_canvas_t *canvas_;
    caca_display_t *display_;
private:
    int frame_;
    caca_event_t event_;
    ostringstream buf_;
    stack<alert_box> alert_boxes_;
    vector<game_event> timed_events_;
    int flags_;

    static const int FLAG_NEEDS_DISPLAY = (1 << 0);
    static const int FLAG_NEEDS_CLEAR = (1 << 1);

    recursive_mutex mutex_;
    recursive_mutex alertsMutex_;
    recursive_mutex eventsMutex_;

    friend class alert_box;
};

#endif
