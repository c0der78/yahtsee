#ifndef _ALERT_BOX_H_
#define _ALERT_BOX_H_

#include <functional>
#include <caca.h>

using namespace std;

class alert_box
{
public:

    alert_box(caca_canvas_t *canvas, caca_display_t *display, int x, int y, int width, int height, std::function<void(const alert_box &)> callback);

    void display();

    int width() const;

    int height() const;

    int x() const;

    int y() const;

    caca_canvas_t *canvas() const;

    void center(const string &text) const;
    void center_x(int y, const string &text) const;

    int center_y() const;

    int center_x() const;

private:
    caca_canvas_t *canvas_;
    caca_display_t *display_;
    int x_, y_;
    int width_, height_;
    std::function<void(const alert_box &)> callback_;
};

#endif
