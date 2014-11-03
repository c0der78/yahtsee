#include "alert_box.h"
#include <string>

alert_box::alert_box(caca_canvas_t *canvas, caca_display_t *display, int x, int y, int width, int height, function<void(const alert_box &)> callback) :
    canvas_(canvas), display_(display), x_(x), y_(y), width_(width), height_(height), callback_(callback)
{
}

void alert_box::display()
{
    caca_fill_box(canvas_, x_, y_, width_, height_, ' ');

    caca_draw_thin_box(canvas_, x_, y_, width_, height_);

    callback_(*this);

    caca_refresh_display(display_);
}

int alert_box::width() const
{
    return width_;
}

int alert_box::height() const
{
    return height_;
}

int alert_box::x() const
{
    return x_;
}

int alert_box::y() const
{
    return y_;
}

caca_canvas_t *alert_box::canvas() const
{
    return canvas_;
}

void alert_box::center(const string &text) const
{
    int x = center_x() - (text.length() / 2);
    int y = center_y();

    caca_put_str(canvas_, x, y, text.c_str());
}
void alert_box::center_x(int y, const string &text) const
{
    int x = center_x() - (text.length() / 2);

    caca_put_str(canvas_, x, y, text.c_str());
}

int alert_box::center_y() const
{
    return y_ + (height_ / 2);
}

int alert_box::center_x() const
{
    return x_ + (width_ / 2);
}