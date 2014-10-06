
#include <stdlib.h>
#include <iostream>
#include <arg3dice/yaht/engine.h>

#include "caca.h"

using namespace arg3::yacht;

using namespace std;

typedef enum
{
    ASK_NAME,
    PLAYING,
    QUIT
} game_state;

class caca_game
{
public:
    caca_game() : state_(ASK_NAME), frame_(1), canvas_buffer_size_(0), canvas_(NULL), display_(NULL)
    {
    }

    ~caca_game()
    {
        reset();
    }

    void reset()
    {
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

        if (canvas_buffer_ != NULL)
        {
            free(canvas_buffer_);
            canvas_buffer_ = NULL;
            canvas_buffer_size_ = 0;
        }
    }

    void start()
    {
        reset();

        canvas_ = caca_create_canvas(0, 0);

        caca_import_canvas_from_file(canvas_, "template.txt", "utf8");

        canvas_buffer_ = caca_export_canvas_to_memory(canvas_, "caca", &canvas_buffer_size_);

        display_ = caca_create_display(canvas_);

        if (display_ == NULL)
        {
            cerr << "Failed to create display" << endl;
            exit(1);
        }

        setState(ASK_NAME);

        caca_set_frame(canvas_, state_);

        prompt();

        caca_refresh_display(display_);
    }

    bool alive() const
    {
        return state_ != QUIT;
    }

    bool hasKeyPress()
    {
        if (display_ == NULL) return false;

        return caca_get_event(display_, CACA_EVENT_KEY_PRESS, &event_, -1) != 0;
    }

    void waitForKeyPress()
    {
        if (display_ == NULL) return;

        while (caca_get_event(display_, CACA_EVENT_KEY_PRESS, &event_, -1) == 0)
            usleep(50000);
    }

    game_state state() const
    {
        return state_;
    }

    void setState(game_state value)
    {
        state_ = value;

        newFrame();
    }

    int readInput()
    {
        return caca_get_event_key_ch(&event_);
    }

    void prompt()
    {
        switch (state_)
        {
        case ASK_NAME:
            caca_put_str(canvas_, 80, 20, "What is your name? ");
            caca_gotoxy(canvas_, 80, 22);
            break;

        default:
            caca_put_str(canvas_, 80, 20, "Command: ");
            caca_gotoxy(canvas_, 80, 22);
            break;
        }

    }

    void refresh(bool reset = false)
    {
        if (reset)
        {
            clear();
        }

        player *player = engine::instance()->current_player();

        if (player != NULL)
        {
            if (reset)
                player->reset();

            put(50, 2, player->name().c_str());

            int y = 8;

            scoresheet::value_type total_score = 0;

            for (int i = 0; i <= Constants::NUM_DICE; i++, y += 2)
            {
                auto score = player->score().upper_score(i + 1);

                put(46, y, std::to_string(score).c_str());

                total_score += score;
            }

            put(46, y, std::to_string(total_score).c_str());

            put(46, y + 2, std::to_string(total_score > 63 ? 35 : 0).c_str());

            auto lower_score_total = total_score;
            if (total_score > 63)
                lower_score_total += 35;

            put(46, y + 4, std::to_string(lower_score_total).c_str());

            total_score = 0;

            y  += 8;

            for (int i = scoresheet::FIRST_TYPE; i < scoresheet::MAX_TYPE; i++)
            {
                scoresheet::type type = static_cast<scoresheet::type>(i);

                auto score = player->score().lower_score(type);

                put(46, y, std::to_string(score).c_str());

                total_score += score;

                switch (type)
                {
                default:
                    y += 2;
                    break;
                case scoresheet::STRAIGHT_SMALL:
                case scoresheet::STRAIGHT_BIG:
                case scoresheet::YACHT:
                    y += 3;
                    break;
                }
            }

            put(46, y, std::to_string(total_score).c_str());

            put(46, y + 2, std::to_string(lower_score_total).c_str());

            put(46, y + 4, std::to_string(total_score + lower_score_total).c_str());
        }

        caca_refresh_display(display_);

    }

    void clear()
    {
        caca_clear_canvas(canvas_);

        setCursor(0, 0);

        caca_import_canvas_from_memory(canvas_, canvas_buffer_, canvas_buffer_size_, "caca");

        setCursor(80, 20);

        prompt();

    }

    void setCursor(int x, int y)
    {
        caca_gotoxy(canvas_, x, y);
    }

    void put(int x, int y, const char *value)
    {
        caca_put_str(canvas_, x, y, value);
    }

    void put(int x, int y, int value)
    {
        caca_put_char(canvas_, x, y, value);
    }

    void newFrame()
    {
        caca_create_frame(canvas_, ++frame_);
    }

    int frames() const
    {
        return frame_;
    }

    size_t addToBuffer(int ch)
    {
        buf_.put(ch);

        return buf_.str().length();
    }

    void clearBuffer()
    {
        buf_.clear();
    }

    string getBuffer()
    {
        return buf_.str();
    }
private:
    game_state state_;
    int frame_;
    size_t canvas_buffer_size_;
    caca_canvas_t *canvas_;
    void *canvas_buffer_;
    caca_display_t *display_;
    caca_event_t event_;
    ostringstream buf_;
};

void display_dice(caca_game &, player *);

int main(int argc, char **argv)
{
    caca_game game;

    game.start();

    while (game.alive())
    {
        if (!game.hasKeyPress())
            continue;

        if (game.state() == ASK_NAME)
        {
            int ch = game.readInput();

            if (ch == CACA_KEY_RETURN)
            {
                string name = game.getBuffer();

                engine::instance()->add_player(name);

                game.setState(PLAYING);

                game.clearBuffer();

                game.clear();
            }
            else
            {
                int len = game.addToBuffer(ch);

                game.put(80 + len - 1, 22, ch);

                game.setCursor(80 + len, 22);
            }

            game.refresh();

            continue;
        }

        auto player = engine::instance()->current_player();

        char input = game.readInput();

        switch (input)
        {
        case 'r':
            if (player->roll_count() < 3)
            {
                player->roll();
                game.newFrame();
                display_dice(game, player);
                game.refresh();
            }
            else
            {
                game.put(80, 28, "You must choose a score after three rolls.");
                game.refresh();
            }
            break;
        case 'd':
        {
            game.waitForKeyPress();

            int d = game.readInput() - '0' - 1;
            player->keep_die(d);
            display_dice(game, player);
            game.refresh();

            break;
        }
        case 'f':
            player->score().lower_score(scoresheet::FULL_HOUSE, player->calculate_lower_score(scoresheet::FULL_HOUSE));
            game.refresh(true);
            break;
        case 'k':
        {
            game.waitForKeyPress();

            int n = game.readInput() - '0';

            if (n == 3)
            {
                player->score().lower_score(scoresheet::KIND_THREE, player->calculate_lower_score(scoresheet::KIND_THREE));
                game.refresh(true);
            }
            else if (n == 4)
            {
                player->score().lower_score(scoresheet::KIND_FOUR, player->calculate_lower_score(scoresheet::KIND_FOUR));
                game.refresh(true);
            }

            break;
        }
        case 's':
        {
            game.waitForKeyPress();

            int n = game.readInput() - '0';

            if (n == 4)
            {
                player->score().lower_score(scoresheet::STRAIGHT_SMALL, player->calculate_lower_score(scoresheet::STRAIGHT_SMALL));
                game.refresh(true);
            }
            else if (n == 5)
            {
                player->score().lower_score(scoresheet::STRAIGHT_BIG, player->calculate_lower_score(scoresheet::STRAIGHT_BIG));
                game.refresh(true);
            }

            break;
        }
        case 'y':
            player->score().lower_score(scoresheet::YACHT, player->calculate_lower_score(scoresheet::YACHT));
            game.refresh(true);
            break;
        case 'c':
            player->score().lower_score(scoresheet::CHANCE, player->calculate_lower_score(scoresheet::CHANCE));
            game.refresh(true);
            break;
        case '1':
            player->score().upper_score(1, player->calculate_upper_score(1));
            game.refresh(true);
            break;
        case '2':
            player->score().upper_score(2, player->calculate_upper_score(2));
            game.refresh(true);
            break;
        case '3':
            player->score().upper_score(3, player->calculate_upper_score(3));
            game.refresh(true);
            break;
        case '4':
            player->score().upper_score(4, player->calculate_upper_score(4));
            game.refresh(true);
            break;
        case '5':
            player->score().upper_score(5, player->calculate_upper_score(5));
            game.refresh(true);
            break;
        case '6':
            player->score().upper_score(6, player->calculate_upper_score(6));
            game.refresh(true);
            break;
        case 'q':
            game.setState(QUIT);
            break;
        }
    }

    return 0;
}

void display_dice(caca_game &game, player *player)
{
    char buf[BUFSIZ + 1] = {0};
    auto dice = player->d1ce();
    snprintf(buf, BUFSIZ, "Roll %d of 3. (Use 'd#' to keep):", player->roll_count());
    game.put(80, 20, buf);
    int x = 82;

    game.put(80, 22, "#  1 │ 2 │ 3 │ 4 │ 5");
    game.put(80, 23, "  ───┴───┴───┴───┴───");
    game.put(80, 24, "  ");
    for (auto value : dice)
    {
        game.put(x++, 24, value.keep() ? '*' : ' ');

        game.put(x++, 24, to_string(value.value()).c_str());

        x += 2;
    }
}
