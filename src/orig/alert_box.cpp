#include "alert_box.h"
#include "caca_game.h"
#include <mutex>
#include <string>

alert_box::alert_box(caca_game *game, dimensional *dimensions, function<void(const alert_box &)> callback) :
    game_(game), dimensions_(dimensions), callback_(callback)
{
}

void alert_box::display()
{
    // grab a lock for canvas manipulation
    unique_lock<recursive_mutex> lock(game_->mutex_);

    // clear out an area for the alert box
    caca_set_color_ansi(game_->canvas_, CACA_WHITE, CACA_TRANSPARENT);

    caca_fill_box(game_->canvas_, x(), y(), width(), height(), ' ');

    // draw a border
    caca_draw_thin_box(game_->canvas_, x(), y(), width(), height());

    caca_set_color_ansi(game_->canvas_, CACA_TRANSPARENT, CACA_TRANSPARENT);

    // call additional drawing
    if (callback_ != nullptr) {
        callback_(*this);
    }
}

int alert_box::width() const
{
    return dimensions_->w();
}

int alert_box::height() const
{
    return dimensions_->h();
}

int alert_box::x() const
{
    return dimensions_->x();
}

int alert_box::y() const
{
    return dimensions_->y();
}

void alert_box::center(const string &text) const
{
    // grab a game canvas lock
    unique_lock<recursive_mutex> lock(game_->mutex_);

    // find the x y coordinates
    int x = center_x() - (text.length() / 2);
    int y = center_y();

    // draw the text
    caca_put_str(game_->canvas_, x, y, text.c_str());
}

void alert_box::center_x(int y, const string &text) const
{
    // grab a game canvas lock
    unique_lock<recursive_mutex> lock(game_->mutex_);

    // find the x position
    int x = center_x() - (text.length() / 2);

    // draw the text
    caca_put_str(game_->canvas_, x, y, text.c_str());
}

int alert_box::center_y() const
{
    return y() + (height() / 2);
}

int alert_box::center_x() const
{
    return x() + (width() / 2);
}

int alert_box::center_x(const string &str) const
{
    return x() + ( (width() - str.length()) / 2);
}