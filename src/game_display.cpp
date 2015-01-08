#include "game.h"
#include "player.h"

using namespace arg3;

void game::display_game_menu()
{

    display_alert([&](const alert_box & a)
    {
        caca_canvas_t *temp = caca_create_canvas(0, 0);

        caca_import_canvas_from_file(temp, resource_file_name((flags_ & FLAG_CONTINUE) ? "continue_menu.txt" : "menu.txt"), "utf8");

        size_t menubufSize;

        void *menubuf = caca_export_canvas_to_memory(temp, "caca", &menubufSize);

        caca_free_canvas(temp);

        caca_import_area_from_memory(canvas_, a.x() + 4, a.y() + 3, menubuf, menubufSize, "caca");
    });
}

void game::display_ask_name()
{
    display_alert([&](const alert_box & a)
    {
        char buf[BUFSIZ + 1] = {0};

        snprintf(buf, BUFSIZ, "What is Player %zu's name?", players_.size() + 1);

        int mod = (strlen(buf) / 2);

        int x = a.center_x();

        int y = a.center_y();

        set_cursor(x - mod, y);

        put(x - mod, y - 1, buf);

        for (int i = x - mod + 8; i < x + mod - 8; i++)
            put_color(i, y - 1, players_.size() + 2);

        for (auto &ch : get_buffer())
        {
            put(x++, y, ch);
        }

    });
}

void game::display_ask_number_of_players()
{
    display_alert("How many players are playing?");
}

void game::display_waiting_for_connections()
{

    display_alert("Waiting for connections...");
}

void game::display_multiplayer_menu()
{
    display_alert([&](const alert_box & a)
    {
        string buf1 = "'h' : host a game";
        string buf2 = "'j' : join a game";

        int xmod = min(buf1.length() / 2, buf2.length() / 2);

        put(a.center_x() - xmod, a.center_y() - 1, buf1.c_str());
        put(a.center_x() - xmod, a.center_y(), buf2.c_str());
    });
}

void game::display_client_waiting_to_start()
{
    char buf[BUFSIZ + 1] = {0};

    vector<string> message;

    int count = 1;

    for (const auto &p : players_)
    {
        snprintf(buf, BUFSIZ, "%2d: %s", count++, p->name().c_str());
        message.push_back(buf);
    }

    message.push_back(" ");

    message.push_back("Waiting for host to start game...");

    display_alert(message);
}


void game::display_already_scored()
{
    display_alert(2000, "You've already scored that.");
}

void game::display_dice(shared_ptr<player> player, int x, int y)
{
    char buf[BUFSIZ + 1] = {0};

    if (players_.size() == 1 || player == this_player())
        snprintf(buf, BUFSIZ, "Roll %d of 3. (Press '#' to keep):", player->roll_count());
    else
        snprintf(buf, BUFSIZ, "%s's roll (%d of 3):", player->name().c_str(), player->roll_count());

    x += 13;
    y += 4;

    int xs = x;

    put(x, y, buf);

    x += 2;
    y += 2;

    put(x, y++, "#  1 │ 2 │ 3 │ 4 │ 5");
    put(x, y++, "  ───┴───┴───┴───┴───");
    put(x++, y, "  ");
    for (size_t i = 0; i < player->die_count(); i++)
    {
        put(++x, y, player->is_kept(i) ? '*' : ' ');

        put(++x, y, to_string(player->d1e(i).value()).c_str());

        x += 2;
    }

    y += 2;

    if (!is_online() || player == this_player())
        put(xs, y, "Press '?' for help on how to score.");
    else
    {
        snprintf(buf, BUFSIZ, "Waiting for %s's turn to finish...", player->name().c_str());
        put(xs, y, buf);
    }
}

void game::display_dice_roll()
{
    display_alert([&](const alert_box & box)
    {
        display_dice(current_player(), box.x(), box.y());
    });
}


void game::display_confirm_quit()
{
    display_alert("Are you sure you want to quit? (Y/n)");
}


void game::display_help()
{
    display_alert([&](const alert_box & a)
    {
        caca_import_area_from_memory(canvas_, a.x() + 4, a.y() + 3, bufs[BUF_HELP], bufSize[BUF_HELP], "caca");
    });
}


void game::display_player_scores()
{
    int x = 46;

    if (current_player())
    {
        set_color(currentPlayer_ + 2);

        put(minimalLower_ ? 21   : 50, 1, current_player()->name().c_str());

        set_color(CACA_DEFAULT);
    }

    int color = 2;

    for (auto &player : players_)
    {
        switch (displayMode_)
        {
        case MINIMAL:
            if (minimalLower_)
            {
                display_lower_scores(color, player->score(), player->calculate_total_upper_score(), x, 3);
            }
            else
            {
                display_upper_scores(color, player->score(), x, 7 );
            }
            break;
        case VERTICAL:
        {
            yaht::scoresheet::value_type lower_score_total = display_upper_scores(color, player->score(), x , 7 );
            display_lower_scores(color, player->score(), lower_score_total, x, 28);
            break;
        }
        case HORIZONTAL:
        {
            yaht::scoresheet::value_type lower_score_total = display_upper_scores(color, player->score(), x , 7 );
            display_lower_scores(color, player->score(), lower_score_total, x + 76, 2);
            break;
        }
        }

        x += 5;

        color++;
    }
}

yaht::scoresheet::value_type game::display_upper_scores(int color, const yaht::scoresheet &score, int x, int y)
{
    yaht::scoresheet::value_type total_score = 0;

    set_color(color);

    for (int i = 0; i <= yaht::Constants::NUM_DICE; i++, y += 2)
    {
        auto value = score.upper_score(i + 1);

        put(x, y, std::to_string(value).c_str());

        total_score += value;
    }

    put(x, y, std::to_string(total_score).c_str());

    put(x, y + 2, std::to_string(total_score > 63 ? 35 : 0).c_str());

    auto lower_score_total = total_score;

    if (total_score > 63)
        lower_score_total += 35;

    put(x, y + 4, std::to_string(lower_score_total).c_str());

    set_color(CACA_DEFAULT);

    return lower_score_total;

}

void game::display_lower_scores(int color, const yaht::scoresheet &score, yaht::scoresheet::value_type lower_score_total, int x, int y)
{
    yaht::scoresheet::value_type total_score = 0;

    set_color(color);

    for (int i = yaht::scoresheet::FIRST_TYPE; i < yaht::scoresheet::MAX_TYPE; i++)
    {
        yaht::scoresheet::type type = static_cast<yaht::scoresheet::type>(i);

        auto value = score.lower_score(type);

        put(x, y, std::to_string(value).c_str());

        total_score += value;

        switch (type)
        {
        default:
            y += 2;
            break;
        case yaht::scoresheet::STRAIGHT_SMALL:
        case yaht::scoresheet::STRAIGHT_BIG:
            y += 3;
            break;
        }
    }

    put(x, y, std::to_string(total_score).c_str());

    put(x, y + 2, std::to_string(lower_score_total).c_str());

    put(x, y + 4, std::to_string(total_score + lower_score_total).c_str());

    set_color(CACA_DEFAULT);

}