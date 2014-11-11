#ifndef _YAHT_GAME_H_
#define _YAHT_GAME_H_

#include "caca_game.h"
#include "matchmaker.h"
#include <arg3dice/yaht/engine.h>

using namespace arg3::yaht;

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

    bool alive() const;

    void refresh_display(bool reset);

    void on_resize(int width, int height);

    void on_quit();

    void on_key_press(int input);

    void action_add_network_player(const string &name);

    void action_remove_network_player(const string &name);

private:

    /* states */

    void state_ask_name(int input);

    void state_playing(int input);

    void state_rolling_dice(int input);

    void state_quit_confirm(int input);

    void state_help_menu(int input);

    void state_game_menu(int input);

    void state_ask_number_of_players(int input);

    void state_multiplayer_menu(int input);

    void state_waiting_for_connections(int input);

    /* display methods */

    void display_alert(int millis, function<void(const alert_box &)> funk);

    void display_alert(function<void(const alert_box &)> funk);

    void display_alert(const string &message);

    void display_alert(int millis, const string &message);

    void display_already_scored();

    void display_dice(player *player, int x, int y);

    void display_help();

    void display_game_menu();

    void display_ask_name();

    void display_dice_roll();

    void display_confirm_quit();

    void display_ask_number_of_players();

    void display_multiplayer_menu();

    scoresheet::value_type display_upper_scores(const scoresheet &score, int x, int y);

    void display_lower_scores(const scoresheet &score, scoresheet::value_type lower_score_total, int x, int y);

    /* actions */

    void action_host_game();

    void action_join_game();

    void action_roll_dice();

    void action_select_die(player *player, int d);

    void action_lower_score(player *player, scoresheet::type type);

    void action_score(player *player, int n);

    void action_score_best(player *player);

    void action_finish_turn();

    /* misc */

    void set_state(state_handler value);

    bool is_state(state_handler value);

    bool is_playing();

    void init_canvas(caca_canvas_t *canvas);

    void set_display_mode(display_mode mode);

    int get_alert_x() const;

    int get_alert_y() const;

    int get_alert_w() const;

    int get_alert_h() const;

    void *upperbuf_, *lowerbuf_, *menubuf_, *headerbuf_, *helpbuf_;
    size_t upperbuf_size_, lowerbuf_size_, menubuf_size_, headerbuf_size_, helpbuf_size_;
    state_handler state_, last_state_;
    display_mode display_mode_;
    bool minimalLower_;
    unsigned num_players_;
    matchmaker matchmaker_;
    int flags_;

    constexpr static const char *HELP = "Type '?' to show command options.  Use the arrow keys to cycle views modes.";

    static const int FLAG_MULTIPLAYER = (1 << 0);
};

#endif
