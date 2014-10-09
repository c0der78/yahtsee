
class caca_game
{
public:
    caca_game() : frame_(1), canvas_buffer_size_(0), canvas_(NULL), display_(NULL)
    {
    }

    ~caca_game()
    {
        reset();
    }

    void reset()
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

        on_start();

        caca_set_frame(canvas_, frame_);

        prompt();

        caca_refresh_display(display_);
    }

    bool has_key_press()
    {
        if (display_ == NULL) return false;

        return caca_get_event(display_, CACA_EVENT_KEY_PRESS, &event_, -1) != 0;
    }

    int read_input()
    {
        return caca_get_event_key_ch(&event_);
    }

    virtual void on_start() {}

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

        caca_import_canvas_from_memory(canvas_, canvas_buffer_, canvas_buffer_size_, "caca");

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
private:
    int frame_;
    size_t canvas_buffer_size_;
    caca_canvas_t *canvas_;
    void *canvas_buffer_;
    caca_display_t *display_;
    caca_event_t event_;
    ostringstream buf_;
};
