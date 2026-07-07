#ifndef HIT_OR_MISS_H
#define HIT_OR_MISS_H

#include "splashkit.h"
#include "Target.h"
#include "global.h"

enum game_states
{
    MENU,
    PLAYING,
    BONUS_GAME,
    MULTIPLAYER,
    MULTIPLAYER_PAUSE,
    PAUSE,
    GAME_OVER,
    MULTIPLAYER_GAME_OVER
};

struct arrow_data
{
    point_2d position;
    double angle;
    int direction;

    bool loose;
    bool arrow_appear;

    timer aim_timer;
    long aim_time;
};

struct score_popup
{
    point_2d position;
    string text;
    color text_color;
    long long start_time;
};

struct player_data
{
    arrow_data arrow;
    int score;
    int hearts;
    double spawn_x;
    long last_time;
    int streak;
};

struct bonus_game_data
{
    point_2d basket;
    int multiplier;
    long last_time;
};

struct game_data
{
    game_states state;
    Target *Targets[MAX_TARGET];
    vector<score_popup> popups;

    player_data players[2];
    player_data single_player;
    int winner;

    timer game_timer;
    long last_time;

    bool state_sound;

    bonus_game_data bonus_game;

};



bool mouse_clicked_button(int botton_x, int botton_y, int botton_width, int botton_height);

void start_game(game_data &game);
void start_multiplayer(game_data &game);
void start_bonus_game(bonus_game_data &bonus);
void draw_game(const game_data &game);
void handle_input(game_data &game);
void update_game(game_data &game);
int get_best_score();

#endif
