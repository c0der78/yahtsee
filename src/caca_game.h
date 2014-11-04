#include <caca.h>
#include <vector>
#include <stack>
#include <thread>
#include <sstream>
#include "alert_box.h"

using namespace std;

class game_event
{
public:
    game_event(unsigned millis, function<void()> callback);
    bool ready() const;
    void perform();
private:
    void run();
    unsigned millis_;
    function<void()> callback_;
    bool ready_;
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

    virtual void refresh_display(bool reset) = 0;

    void refresh(bool reset = false);

    void clear();

    void set_cursor(int x, int y);

    int get_cursor_x() const;

    int get_cursor_y() const;

    void put(int x, int y, const char *value);

    void put(int x, int y, int value);

    void display_alert(int x, int y, int width, int height, function<void(const alert_box &)> callback);

    alert_box displayed_alert() const;

    bool has_alert() const;

    void pop_alert();

    void pop_alert(int millis, std::function<void()> funk = nullptr);

    void new_frame();

    void pop_frame();

    int frames() const;

    size_t add_to_buffer(int ch);

    void clear_buffer();

    string get_buffer();

    void add_event(unsigned millis, function<void()> callback);
protected:

    virtual void init_canvas(caca_canvas_t *canvas) = 0;

private:
    int frame_;
    caca_canvas_t *canvas_;
    caca_display_t *display_;
    caca_event_t event_;
    ostringstream buf_;
    stack<alert_box> alert_boxes_;
    vector <game_event> timed_events_;
};
