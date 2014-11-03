
#include "caca_game.h"

#include <arg3dice/yaht/engine.h>

using namespace arg3::yaht;

/*
typedef enum
{
    ASK_NAME,
    PLAYING,
    ROLLING_DICE,
    DISPLAY_MENU,
    QUIT,
    QUIT_CONFIRM
} game_state;*/

typedef enum
{
    HORIZONTAL,
    VERTICAL,
    MINIMAL
} display_mode;


class yaht_game : public caca_game
{

public:

    typedef void (yaht_game::*state_handler)(int);

    typedef std::function<void(int)> game_state;

    yaht_game();

    void reset();

    state_handler state() const;

    void recover_state();

    void on_start();

    void prompt();

    bool alive() const;

    void refresh_display(bool reset);

    void on_resize(int width, int height);

    void on_quit();

    void on_key_press(int input);

private:

    /* states */

    void state_ask_name(int ch);

    void state_playing(int input);

    void state_rolling_dice(int input);

    void state_quit_confirm(int input);

    void state_display_menu(int ch);

    /* end states */

    /* display methods */

    void display_alert(function<void(const alert_box &a)> funk);

    void display_already_scored();

    void display_dice(player *player, int x, int y);

    void display_menu();

    scoresheet::value_type display_upper_scores(const scoresheet &score, int x, int y);

    void display_lower_scores(const scoresheet &score, scoresheet::value_type lower_score_total, int x, int y);

    /* actions */

    void action_display_dice();

    void action_roll_dice();

    void action_select_die(player *player, int d);

    void action_lower_score(player *player, scoresheet::type type);

    void action_score(player *player, int n);

    void action_score_best(player *player);

    void action_confirm_quit();


    /* end actions */

    void set_state(state_handler value);

    bool is_state(state_handler value);

    bool is_playing();

    void init_canvas(caca_canvas_t *canvas);

    void set_display_mode(display_mode mode);

    int get_alert_x() const;

    int get_alert_y() const;

    int get_alert_w() const;

    int get_alert_h() const;

    void *upperbuf_, *lowerbuf_, *menubuf_, *headerbuf_;
    size_t upperbuf_size_, lowerbuf_size_, menubuf_size_, headerbuf_size_;
    state_handler state_, last_state_;
    display_mode display_mode_;
    bool minimalLower_;


    constexpr static const char *HELP = "Type '?' to show command options.  Use the arrow keys to cycle views modes.";

};
