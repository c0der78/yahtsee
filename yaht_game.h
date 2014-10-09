
#include "caca_game.h"

class yaht_game : public caca_game
{
public:
    game_state state() const
    {
        return state_;
    }

    void set_state(game_state value)
    {
        state_ = value;

        newFrame();
    }

    void on_start()
    {

        set_state(ASK_NAME);
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

    bool alive() const
    {
        return state_ != QUIT;
    }

    void refresh_display()
    {
        player *player = engine::instance()->current_player();

        if (player != NULL)
        {
            if (reset)
                player->reset();

            put(50, 2, player->name().c_str());

            int y = 8;

            scoresheet::value_type total_score = 0;

            for (int i = 0; i <= Constants::NUM_DICE; i++, y += 2)
            {
                auto score = player->score().upper_score(i + 1);

                put(46, y, std::to_string(score).c_str());

                total_score += score;
            }

            put(46, y, std::to_string(total_score).c_str());

            put(46, y + 2, std::to_string(total_score > 63 ? 35 : 0).c_str());

            auto lower_score_total = total_score;

            if (total_score > 63)
                lower_score_total += 35;

            put(46, y + 4, std::to_string(lower_score_total).c_str());

            total_score = 0;

            y  += 8;

            for (int i = scoresheet::FIRST_TYPE; i < scoresheet::MAX_TYPE; i++)
            {
                scoresheet::type type = static_cast<scoresheet::type>(i);

                auto score = player->score().lower_score(type);

                put(46, y, std::to_string(score).c_str());

                total_score += score;

                switch (type)
                {
                default:
                    y += 2;
                    break;
                case scoresheet::STRAIGHT_SMALL:
                case scoresheet::STRAIGHT_BIG:
                case scoresheet::YACHT:
                    y += 3;
                    break;
                }
            }

            put(46, y, std::to_string(total_score).c_str());

            put(46, y + 2, std::to_string(lower_score_total).c_str());

            put(46, y + 4, std::to_string(total_score + lower_score_total).c_str());
        }

    }
private:

    game_state state_;
};
