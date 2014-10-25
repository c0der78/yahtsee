
class alert_box
{
public:

    alert_box(caca_canvas_t *canvas, caca_display_t *display, int width, int height, function<void(const alert_box &, int, int)> callback) :
        canvas_(canvas), display_(display), width_(width), height_(height), callback_(callback)
    {
    }

    void display(int x, int y)
    {
        caca_fill_box(canvas_, x, y, width_, height_, ' ');

        caca_draw_thin_box(canvas_, x, y, width_, height_);

        callback_(*this, x, y);

        caca_refresh_display(display_);
    }

    void display()
    {
        int x = (caca_get_canvas_width(canvas_) / 2) - (width_ / 2);
        int y = (caca_get_canvas_height(canvas_) / 2) - (height_ / 2);

        display(x, y);
    }

    int width() const
    {
        return width_;
    }

    int height() const
    {
        return height_;
    }

    caca_canvas_t *canvas() const
    {
        return canvas_;
    }

private:
    caca_canvas_t *canvas_;
    caca_display_t *display_;
    int width_, height_;
    function<void(const alert_box &, int, int)> callback_;
};
