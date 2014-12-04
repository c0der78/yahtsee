#ifndef _ALERT_BOX_H_
#define _ALERT_BOX_H_

#include <functional>
#include <caca.h>

using namespace std;

class caca_game;

class alert_box
{
public:

    void display();

    int width() const;

    int height() const;

    int x() const;

    int y() const;

    void center(const string &text) const;

    void center_x(int y, const string &text) const;

    int center_y() const;

    int center_x() const;

    int center_x(const string &text) const;
private:

    alert_box(caca_game *game, int x, int y, int width, int height, std::function<void(const alert_box &)> callback);

    caca_game *game_;
    int x_, y_;
    int width_, height_;
    std::function<void(const alert_box &)> callback_;

    friend class caca_game;
};

#endif
