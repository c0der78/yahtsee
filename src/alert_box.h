#ifndef _ALERT_BOX_H_
#define _ALERT_BOX_H_

#include <functional>
#include <caca.h>

using namespace std;

class caca_game;

/*!
 * Specifies dimensions for the alert box or screen
 */
class dimensional
{
public:
    /*! the x coordinate */
    virtual int x() const = 0;
    /*! the y coordinate */
    virtual int y() const = 0;
    /*! the width */
    virtual int w() const = 0;
    /*! the height */
    virtual int h() const = 0;
};

/*!
 * a simple box overlay with a message
 */
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

    alert_box(caca_game *game, dimensional *dimensions, std::function<void(const alert_box &)> callback);

    caca_game *game_;
    dimensional *dimensions_;
    std::function<void(const alert_box &)> callback_;

    friend class caca_game;
};

#endif
