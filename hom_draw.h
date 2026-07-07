#ifndef HOM_DRAW_H
#define HOM_DRAW_H

#include "splashkit.h"
#include "hit_or_miss.h"

double get_middle_x(string text, font text_font, int text_size);
void draw_arrow(game_data &game);
void draw_target(game_data &game);
void draw_popups(const game_data &game);
void draw_basket(const game_data &game);
void draw_menu(game_data game);
void draw_play(game_data game);
void draw_bonus_game(game_data game);
void draw_multiplayer(game_data game);
void draw_pause(game_data game);
void draw_multiplayer_pause(game_data game);
void draw_game_over(game_data game);
void draw_multiplayer_game_over(game_data game);

#endif