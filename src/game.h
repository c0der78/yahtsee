#ifndef _YAHT_GAME_H_
#define _YAHT_GAME_H_

#include "caca_game.h"
#include "matchmaker.h"

#include <stack>

/*! the mode of display */
typedef enum
{
    HORIZONTAL,
    VERTICAL,
    MINIMAL
} display_mode;

/*! memory buffers */
enum
{
    BUF_LOWER_HEADER,
    BUF_LOWER_HEADER_MINIMAL,
    BUF_UPPER,
    BUF_LOWER,
    BUF_HELP,
    BUF_MENU,
    BUF_MAX
};

class player;

/*! a game of yahtsee */
class game : public caca_game
{

public:

    /*! a state handler */
    typedef void (game::*state_handler)(int);

    game();

    void reset();

    /*! removes the top state */
    void pop_state();

    void on_start();

    /*! game is still being played */
    bool alive() const;

    void on_display();

    void on_resize(int width, int height);

    void on_quit();

    void on_key_press(int input);

    /*! gets the current player */
    shared_ptr<player> current_player() const;

    /*! perform an action for each player */
    void for_players(std::function<bool(const std::shared_ptr<player> &p)> funk);

    /*! add a player to the game */
    void add_player(const shared_ptr<player> &p);

    /*! sets the current player */
    void set_current_player(const shared_ptr<player> &p);

    shared_ptr<player> get_player(size_t index) const;

    shared_ptr<player> this_player() const;

    bool is_online() const;

    bool is_online_available() const;

    const arg3::json::object &settings() const;

    void load_settings(char *exe);

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

    void state_multiplayer_join(int input);

    void state_multiplayer_join_game(int input);

    void state_waiting_for_connections(int input);

    void state_client_waiting_to_start(int input);

    void state_joining_game(int input);

    void state_joining_online_game(int input);

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

    void display_multiplayer_join();

    void display_multiplayer_join_game();

    void display_player_scores();

    arg3::yaht::scoresheet::value_type display_upper_scores(int color, const arg3::yaht::scoresheet &score, int x, int y);

    void display_lower_scores(int color, const arg3::yaht::scoresheet &score, arg3::yaht::scoresheet::value_type lower_score_total, int x, int y);

    void display_client_waiting_to_start();

    void display_waiting_for_connections();

    void display_joining_game();

    /* state initializers/finializers */
    void exit_multiplayer();

    void exit_game();

    void clear_states();

    void stop_playing();

    void init_playing();

    /* actions */

    void action_add_network_player(const shared_ptr<player> &p);

    void action_remove_network_player(connection *c);

    void action_joined_game();

    void action_host_game();

    void action_join_game();

    void action_join_online_game();

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

    void action_game_over();

    /* misc */

    void set_state(state_handler value);

    bool is_state(state_handler value);

    bool is_playing();

    void init_canvas(caca_canvas_t *canvas);

    void set_display_mode(display_mode mode);

    void next_player();

    char *resource_file_name(const char *path, const char *dir = NULL);

    void load_buf(const char *fileName, int index);

    shared_ptr<player> find_player_by_id(const string &id) const;

    /*! a state representation */
    typedef struct
    {
        /*! called when the state starts */
        void (game::*on_init)();
        /*! called when the state has input */
        void (game::*on_execute)(int);
        /*! called when the state is displayed */
        void (game::*on_display)();
        /*! called when the state is finished */
        void (game::*on_exit)();
        /*! flags to determine behaviour */
        int flags;
    } game_state;

    /*! gets alert dimensions specific to the game */
    class alert_dimensions : public dimensional
    {
    public:
        alert_dimensions(game *game);

        int x() const;
        int y() const;
        int w() const;
        int h() const;
    private:
        game *game_;
    };

    /*! the table with states */
    static const game_state state_table[];

    /*! lookup a state in the table */
    static const game_state *find_state(state_handler value);

    void *bufs[BUF_MAX];
    size_t bufSize[BUF_MAX];
    stack<const game_state *> states_;
    display_mode displayMode_;
    bool minimalLower_;
    unsigned numPlayers_;
    matchmaker matchmaker_;
    int flags_;

    recursive_mutex mutex_;

    vector<shared_ptr<player>> players_;
    int currentPlayer_;

    arg3::json::object settings_;

    /*! hosting a network game */
    static const int FLAG_HOSTING = (1 << 0);
    /*! joining a network game */
    static const int FLAG_JOINING = (1 << 1);
    /*! waiting for a network player's turn to finish */
    static const int FLAG_WAITING_FOR_TURN = (1 << 2);
    /*! menu was displayed, but we're continuing the game */
    static const int FLAG_CONTINUE = (1 << 3);
    /*! this player is rolling the dice */
    static const int FLAG_ROLLING = (1 << 4);
    /*! network game is local area network */
    static const int FLAG_LAN = (1 << 5);

    /*! the state should not remain on the stack */
    static const int FLAG_STATE_TRANSIENT = (1 << 0);
    /*! the state is not able to be escaped */
    static const int FLAG_STATE_FORCE = (1 << 1);

    friend class connection;
    friend class client;
};

#endif
