/*
 *  spritedit     sprite editor for libcaca
 *  Copyright (c) 2003-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#include <stdlib.h>
#include <iostream>
#include <arg3/dice/yacht/engine.h>

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

    void refresh()
    {
        Player *player = Engine::instance()->currentPlayer();

        if (player != NULL)
            put(45, 2, player->name().c_str());

        caca_refresh_display(display_);

    }

    void clear()
    {
        caca_clear_canvas(canvas_);

        setCursor(0, 0);

        caca_import_canvas_from_file(canvas_, "template.txt", "utf8");

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

void display_dice(caca_game &, Player *);

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

                Engine::instance()->addPlayer(name);

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

        auto player = Engine::instance()->currentPlayer();

        switch (game.readInput())
        {
        case 'r':
            player->roll();
            game.newFrame();
            display_dice(game, player);
            game.refresh();
            break;
        case 'd':
        {
            if (game.hasKeyPress())
            {
                int d = game.readInput() - '0' - 1;
                player->keepDie(d);
                display_dice(game, player);
                game.refresh();
            }
            break;
        }
        case 'q':
            game.setState(QUIT);
            break;
        }
    }

    return 0;
}

void display_dice(caca_game &game, Player *player)
{
    auto dice = player->dice();
    game.put(80, 20, "Dice rolled ('d#' to keep):");
    int x = 82;

    game.put(80, 22, "#  1 │ 2 │ 3 │ 4 │ 5");
    game.put(80, 23, "  ───┴───┴───┴───┴───");
    game.put(80, 24, "d ");
    for (auto value : dice)
    {
        game.put(x++, 24, value.keep() ? '*' : ' ');

        game.put(x++, 24, to_string(value.value()).c_str());

        x += 2;
    }
}