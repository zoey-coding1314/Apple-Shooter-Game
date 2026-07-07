#include "global.h"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

const int NORMAL_ROTATING_SPEED = 90; // 90 degree per second
const int SLOW_ROTATING_SPEED = 60;
const int ARROW_SPEED = 600; // pixels per second
const int POP_UP_MOVING = 50;
const int BONUS_LIMIT = 10; //Bonus game time limit (s)
const int BONUS_SPEED = 450; //Target falliing speed

const int MIN_ANGLE = -90;
const int MAX_ANGLE = 90;
const int MAX_HEARTS = 3;
const int BONUS_TRIGGER = 3;

const int BUTTON_X = 390;
const int BUTTON_Y = 480;
const int BUTTON_WIDTH = 240;
const int BUTTON_HEIGHT = 110;
const int LINE1_Y = 200;
const int LINE2_Y = 400;

const int TOP_SCORE = 20;
const int MID_SCORE = 15;
const int LOW_SCORE = 10;
const int BONUS_SCORE = 5;

const font FONT1 = load_font("Font1", "Kid Games.ttf");
const font FONT2 = load_font("Font2", "game_over.ttf");
const int FONT_BIG = 100;
const int FONT_SMALL = 50;

bitmap heart_shape = load_bitmap("Heart", "heart_pixel.png");
bitmap pause_image = load_bitmap("Pause", "Pause.png");
bitmap arrow_shape = load_bitmap("Arrow", "Arrow.png");
bitmap apple_shape = load_bitmap("Apple", "apple.png");
bitmap gold_apple = load_bitmap("Gold Apple", "GoldenApple.png");
bitmap bomb_shape = load_bitmap("Bomb", "Bomb.png");
bitmap basket_shape = load_bitmap("Basket", "Basket.png");

sound_effect loose_sound = load_sound_effect("Loose sound", "LooseSound.ogg");
sound_effect normal_hit = load_sound_effect("Normal hit", "NormalHit.ogg");
sound_effect tap_sound = load_sound_effect("Tap sound", "Tap.ogg");
sound_effect gold_hit = load_sound_effect("Gold Hit", "GoldHit.ogg");
sound_effect bomb_hit = load_sound_effect("Bomb Hit", "Hit Bomb.ogg");
sound_effect pause_sound = load_sound_effect("Pause sound", "Pause.ogg");
sound_effect game_over = load_sound_effect("Game over", "GameOver.ogg");

music background_music = load_music("Background music", "Background.ogg");