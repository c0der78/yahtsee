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

/*! represents a timed event in the game */
class game_event
{
public:
    /*! default constructor
     * @param   unsigned    the number of milliseconds to wait
     * @param   function    the callback after the wait
     */
    game_event(unsigned millis, const function<void()> callback);
    /*! non-copyable */
    game_event(const game_event &) = delete;
    game_event(game_event  &&e);
    ~game_event();
    /*! non-copyable */
    game_event &operator=(const game_event &) = delete;
    game_event &operator=(game_event && );

    /*! test if the event is finished waiting */
    bool ready() const;
    /*! perform the event action */
    void perform() const;
    /*! clear this event */
    void clear();
private:
    void wait();
    unsigned millis_;
    function<void()> callback_;
    bool ready_;
    thread worker_;
};

/*! base class for a game using libcaca */
class caca_game
{
public:
    caca_game();

    virtual ~caca_game();

    /*! resets the game and releases resources */
    virtual void reset();

    /*! inits the display and canvas */
    void start();

    /*! updates the input, events and display */
    void update();

    /*! callback when the user exits the game */
    virtual void on_quit() = 0;

    /*! callback when the display is resized */
    virtual void on_resize(int width, int height) = 0;

    /*! callback when a key is pressed */
    virtual void on_key_press(int input) = 0;

    /*! callback when the game starts */
    virtual void on_start() = 0;

    /*! callback when the display is updated */
    virtual void on_display() = 0;

    /*! resets the display */
    void clear_display();

    /*! sets the cursor */
    void set_cursor(int x, int y);

    /*! gets the cursor x position */
    int get_cursor_x() const;

    /*! gets the cursor y position */
    int get_cursor_y() const;

    /*! sets the current foreground color */
    void set_color(int fg);

    /*! puts a color at a position */
    void put_color(int x, int y, int fg);

    /*! puts text at a position */
    void put(int x, int y, const char *value);

    /*! puts a character at a position */
    void put(int x, int y, int value);

    /*! displays an alert on the display */
    void display_alert(dimensional *dimensions, const function<void(const alert_box &)> callback);

    /*! gets the currently displayed alert */
    const alert_box &displayed_alert() const;

    /*! has an alert set */
    bool has_alert() const;

    /*! remove the top alert */
    void pop_alert();

    /*! remove the top alert after a time period */
    void pop_alert(int millis, const std::function<void()> funk = nullptr);

    /*! remove all alerts */
    void clear_alerts();

    /*! remove all events */
    void clear_events();

    /* animation frame methods */

    /*! add a frame to the display */
    void new_frame();

    /*! remove the top frame from the display */
    void pop_frame();

    /*! returns the number of frames in the display */
    int frames() const;

    /*! adds a character to the temp buffer */
    size_t add_to_buffer(int ch);

    /*! clears the temp buffer */
    void clear_buffer();

    /*! pop the last character on the temp buffer */
    void pop_from_buffer();

    /*! get the temp buffer */
    string get_buffer();

    /*! adds an event to the game */
    void add_event(unsigned millis, const function<void()> callback);

    /*! tells the displa it needs an update */
    void set_needs_display();

    /*! tells the display it needs to clear itself */
    void set_needs_clear();
protected:

    /*! initializes the canvas. use to perform additional operations */
    virtual void init_canvas(caca_canvas_t *canvas) = 0;

    caca_canvas_t *canvas_;
    caca_display_t *display_;
private:

    void update_display();

    void update_input();

    void update_events();

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
