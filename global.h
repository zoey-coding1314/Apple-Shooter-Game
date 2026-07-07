#ifndef GLOBAL_H
#define GLOBAL_H

#pragma once
#include "splashkit.h"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

extern const int NORMAL_ROTATING_SPEED;
extern const int SLOW_ROTATING_SPEED;
extern const int ARROW_SPEED;
extern const int POP_UP_MOVING;
extern const int BONUS_LIMIT;
extern const int BONUS_SPEED;

extern const int MAX_HEARTS;
extern const int MIN_ANGLE;
extern const int MAX_ANGLE;
extern const int BONUS_TRIGGER;
constexpr int MAX_TARGET = 5; //Compile-time constant

extern const int BUTTON_X;
extern const int BUTTON_Y;
extern const int BUTTON_WIDTH;
extern const int BUTTON_HEIGHT;
extern const int LINE1_Y;
extern const int LINE2_Y;

extern const int TOP_SCORE;
extern const int MID_SCORE;
extern const int LOW_SCORE;
extern const int BONUS_SCORE;

extern const font FONT1;
extern const font FONT2;
extern const int FONT_BIG;
extern const int FONT_SMALL;

extern bitmap heart_shape;
extern bitmap pause_image;
extern bitmap arrow_shape;
extern bitmap apple_shape;
extern bitmap gold_apple;
extern bitmap bomb_shape;
extern bitmap basket_shape;

extern sound_effect loose_sound;
extern sound_effect normal_hit;
extern sound_effect tap_sound;
extern sound_effect gold_hit;
extern sound_effect bomb_hit;
extern sound_effect pause_sound;
extern sound_effect game_over;
extern music background_music;
#endif
