
class caca_game
{
public:
    caca_game() : frame_(1), canvas_(NULL), display_(NULL)
    {
    }

    virtual ~caca_game()
    {
        reset();
    }

    virtual void reset()
    {
        frame_ = 1;

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

    }

    void start()
    {
        reset();

        canvas_ = caca_create_canvas(0, 0);

        display_ = caca_create_display(canvas_);

        if (display_ == NULL)
        {
            cerr << "Failed to create display" << endl;
            exit(1);
        }

        init_canvas(canvas_);

        caca_set_frame(canvas_, frame_);

        on_start();

        prompt();

        caca_refresh_display(display_);
    }

    void update()
    {
        if (display_ == NULL) return;

        if (caca_get_event(display_, CACA_EVENT_QUIT | CACA_EVENT_RESIZE | CACA_EVENT_KEY_RELEASE, &event_, -1) != 0)
        {
            if (caca_get_event_type(&event_) & CACA_EVENT_QUIT)
            {
                on_quit();
                return;
            }

            if (caca_get_event_type(&event_) & CACA_EVENT_RESIZE)
            {
                int height = caca_get_event_resize_height(&event_);
                int width = caca_get_event_resize_width(&event_);

                on_resize(width, height);

                prompt();

                caca_refresh_display(display_);
            }

            if (caca_get_event_type(&event_) & CACA_EVENT_KEY_RELEASE)
            {
                int input = caca_get_event_key_ch(&event_);

                on_key_press(input);
            }
        }

    }

    virtual void on_quit() = 0;

    virtual void on_resize(int width, int height) = 0;

    virtual void on_key_press(int input) = 0;

    virtual void on_start() = 0;

    virtual void prompt() = 0;

    virtual void refresh_display(bool reset) = 0;

    void refresh(bool reset = false)
    {
        if (reset)
        {
            clear();
        }

        refresh_display(reset);

        caca_refresh_display(display_);

    }

    void clear()
    {
        caca_clear_canvas(canvas_);

        set_cursor(0, 0);

        init_canvas(canvas_);

        set_cursor(80, 20);

        prompt();
    }

    void set_cursor(int x, int y)
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

    void new_frame()
    {
        caca_create_frame(canvas_, ++frame_);
    }


    int frames() const
    {
        return frame_;
    }

    size_t add_to_buffer(int ch)
    {
        buf_.put(ch);

        return buf_.str().length();
    }

    void clear_buffer()
    {
        buf_.str("");
    }

    string get_buffer()
    {
        return buf_.str();
    }
protected:

    virtual void init_canvas(caca_canvas_t *canvas) = 0;

private:
    int frame_;
    caca_canvas_t *canvas_;
    caca_display_t *display_;
    caca_event_t event_;
    ostringstream buf_;
};
