#ifndef HOM_UPDATE_H
#define HOM_UPDATE_H

#include "splashkit.h"
#include "hit_or_miss.h"

arrow_data new_arrow();
void create_random_target(game_data &game, int i);
void create_bonus_target(game_data &game, int i);
void recreate_target(game_data &game, int i);

bool arrow_hit_target(const arrow_data &arrow, const Target &target, bitmap target_shape);
void arrow_rotating(arrow_data &arrow, double seconds);
void update_arrow(player_data &player, game_data &game);
void update_popups(game_data &game, long current_time);

#endif