#ifndef _ALERT_BOX_H_
#define _ALERT_BOX_H_

#include <functional>
#include <caca.h>

using namespace std;

class caca_game;

/*!
 * Specifies dimensions for the alert box or ui entity
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
 * a simple box overlay with some text
 */
class alert_box
{
public:

    /*! displays the alert box */
    void display();

    /*! the width of the box */
    int width() const;

    /*! the height of the box */
    int height() const;

    /*! the x position */
    int x() const;

    /*! the y position */
    int y() const;

    /*! centers some text in the alert box */
    void center(const string &text) const;

    /*! horizontally center some text in the alert box */
    void center_x(int y, const string &text) const;

    /*! get the center y position */
    int center_y() const;

    /*! get the center x position */
    int center_x() const;

    /*! get the center x position for some text */
    int center_x(const string &text) const;
private:

    /*! creates an alert box for the game dimensions with a callback */
    alert_box(caca_game *game, dimensional *dimensions, std::function<void(const alert_box &)> callback);

    caca_game *game_;
    dimensional *dimensions_;
    std::function<void(const alert_box &)> callback_;

    friend class caca_game;
};

#endif
