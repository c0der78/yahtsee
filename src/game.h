#ifndef _YAHT_GAME_H_
#define _YAHT_GAME_H_

#include "caca_game.h"
#include "matchmaker.h"


typedef enum
{
    HORIZONTAL,
    VERTICAL,
    MINIMAL
} display_mode;

class player;

class game : public caca_game
{

public:

    typedef void (game::*state_handler)(int);

    game();

    void reset();

    void recover_state();

    void on_start();

    bool alive() const;

    void on_display();

    void on_resize(int width, int height);

    void on_quit();

    void on_key_press(int input);

    shared_ptr<player> current_player() const;

    void for_players(std::function<void(const std::shared_ptr<player> &p)> funk);

    void add_player(const shared_ptr<player> &p);

    void set_current_player(const shared_ptr<player> &p);

    shared_ptr<player> get_player(size_t index) const;

    shared_ptr<player> this_player() const;

    bool is_online() const;
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

    void display_alert(int millis, const function<void(const alert_box &)> funk, const function<void()> pop = nullptr);

    void display_alert(const function<void(const alert_box &)> funk);

    void display_alert(const string &message, const function<void(const alert_box &a)> funk = nullptr);

    void display_alert(int millis, const string &message, const function<void(const alert_box &a)> funk = nullptr, const function<void()> pop = nullptr);

    void display_alert(const vector<string> &messages, const function<void(const alert_box &a)> funk = nullptr);

    void display_alert(int millis, const vector<string> &messages, const function<void(const alert_box &a)> funk = nullptr, const function<void()> pop = nullptr);

    void display_already_scored();

    void display_dice(shared_ptr<player> player, int x, int y);

    void display_help();

    void display_game_menu();

    void display_ask_name();

    void display_dice_roll();

    void display_confirm_quit();

    void display_ask_number_of_players();

    void display_multiplayer_menu();

    void display_player_scores();

    arg3::yaht::scoresheet::value_type display_upper_scores(const arg3::yaht::scoresheet &score, int x, int y);

    void display_lower_scores(const arg3::yaht::scoresheet &score, arg3::yaht::scoresheet::value_type lower_score_total, int x, int y);

    void display_client_waiting_to_start();

    void display_waiting_for_connections();

    /* state initializers/destructors */
    void exit_multiplayer();

    /* actions */

    void action_add_network_player(const shared_ptr<player> &p);

    void action_remove_network_player(connection *c);

    void action_joined_game();

    void action_host_game();

    void action_join_game();

    void action_network_player_joined(const shared_ptr<player> &p);

    void action_network_player_left(const shared_ptr<player> &p);

    void action_roll_dice();

    void action_select_die(shared_ptr<arg3::yaht::player> player, int d);

    void action_lower_score(shared_ptr<arg3::yaht::player> player, arg3::yaht::scoresheet::type type);

    void action_score(shared_ptr<arg3::yaht::player> player, int n);

    void action_score_best(shared_ptr<arg3::yaht::player> player);

    void action_finish_turn();

    void action_disconnect();

    void action_network_player_finished(const shared_ptr<player> &p);

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

    void next_player();

    shared_ptr<player> find_player_by_id(const string &id) const;

    typedef struct
    {
        void (game::*on_init)();
        void (game::*on_execute)(int);
        void (game::*on_display)();
        void (game::*on_exit)();
    } game_state;

    static const game_state state_table[];

    static const game_state *find_state(state_handler value);

    void *upperbuf_, *lowerbuf_, *menubuf_, *headerbuf_, *helpbuf_;
    size_t upperbufSize_, lowerbufSize_, menubufSize_, headerbufSize_, helpbufSize_;
    const game_state *state_, *lastState_;
    display_mode displayMode_;
    bool minimalLower_;
    unsigned numPlayers_;
    matchmaker matchmaker_;
    int flags_;

    vector<shared_ptr<player>> players_;
    int currentPlayer_;

    static const int FLAG_HOSTING = (1 << 0);
    static const int FLAG_JOINING = (1 << 1);
    static const int FLAG_WAITING_FOR_TURN = (1 << 2);

    friend class connection;
    friend class client;
};

#endif
