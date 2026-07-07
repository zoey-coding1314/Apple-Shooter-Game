#include "splashkit.h"
#include "hit_or_miss.h"
#include "Target.h"
#include "global.h"

using std::to_string;

/** 
 * Checks if it's time for the target to appear
 * 
 * @param current_time The current time in milliseconds
 * @return true if it's time to appear, otherwise false
 */
bool Target::time_to_appear(long current_time)
{
    return (!appear && current_time > appear_time);
}

/**
 * Checks if it's time for the target to escape
 * 
 * @param current_time The current time in milliseconds
 * @return true if it's time to escape, otherwise false
 */
bool Target::time_to_escape(long current_time)
{
    return (appear && current_time > escape_time);
}

/**
 * Update the target based on time to appear and dissapear.
 */
void Target::update(long current_time)
{
    if (time_to_appear(current_time))
    {
        appear = true;
        escape_time = current_time + 2000 + rnd(3000, 4000);
        expired = false;
    }
    else if (time_to_escape(current_time))
    {
        appear = false;
        appear_time = current_time + 1000 + rnd(2000);
        expired = true;
    }
}

/**
 * Change the y position of the target to -20
 * Use for bonus game to relocate the targats at the top of the screen
 */
void Target::initial()
{
    position.y = -20;
    appear = true;
}

/**
 * Update the target's position, creates visually falling down target.
 * If the target gets out of the screen, it is expired.
 * 
 * @param second The past time used to calculate the visually moving pace.
 */
void Target::bonus_update(double second)
{
    if(position.y < SCREEN_HEIGHT)
    {
        position.y += (second * BONUS_SPEED);
    }
    else
    {
        appear = false;
        expired = true;
    }
    
}

/**
 * Create an apple target at desired location for testing purpose.
 */
Apple::Apple()
{
    position.x = rnd(0, SCREEN_WIDTH - bitmap_width(apple_shape));
    position.y = rnd(0, SCREEN_HEIGHT - 200 - bitmap_width(apple_shape));
    appear = false;
    appear_time = current_ticks() + 1000 + rnd(2000);
    escape_time = 0;
    target_shape = apple_shape;
    expired = false;
}

/**
 * Create an apple target at desired location for testing purpose.
 * 
 * @param x The position x of the apple
 * @param y The position y of the apple
 */
Apple::Apple(int x, int y)
{
    position.x = x;
    position.y = y;
    appear = false;
    appear_time = current_ticks() + 1000 + rnd(2000);
    escape_time = 0;
    target_shape = apple_shape;
    expired = false;
}

/** 
 * Creates a new Apple with a random position
 * 
 * @param current_time The time passed after the timer starts, used to calculate appear time
 */
Apple::Apple(long current_time)
{
    position.x = rnd(0, SCREEN_WIDTH - bitmap_width(apple_shape));
    position.y = rnd(0, SCREEN_HEIGHT - 200 - bitmap_width(apple_shape));
    appear = false;
    appear_time = current_time + 1000 + rnd(2000);
    escape_time = 0;
    target_shape = apple_shape;
    expired = false;
}

/** 
 * Draws the apple on the screen
 */
void Apple::draw()
{
    if(appear)
    {
        draw_bitmap(apple_shape, position.x, position.y);
    }
    
}

/** 
 * Manages the collision of the apple, updating score and creating popup
 * 
 * @param player The player data for updating
 * @param game The current game data
 */
void Apple::hit(player_data &player, game_data &game)
{
    play_sound_effect(normal_hit);
    score_popup popup;
    string text;
    if(position.y + bitmap_height(apple_shape)/2 <= LINE1_Y) //Check if the at least half the apple is in the top zone
    {
        player.score += TOP_SCORE;
        text = "+" + to_string(TOP_SCORE);
    }
    else if(position.y + bitmap_height(apple_shape)/2 >= LINE1_Y && position.y + bitmap_height(apple_shape)/2 < LINE2_Y)
    {
        player.score += MID_SCORE;
        text = "+" + to_string(MID_SCORE);
    }
    else
    {
        player.score += LOW_SCORE;
        text = "+" + to_string(LOW_SCORE);
    }
    appear = false;
    game.popups.push_back(score_popup{position, text, COLOR_LIME_GREEN, current_ticks()});
}

/** 
 * Creates a new Golden Apple with a random position
 * 
 * @param current_time The time passed after the timer starts, used to calculate appear time
 */
GoldenApple::GoldenApple(long current_time)
{
    position.x = rnd(0, SCREEN_WIDTH - bitmap_width(gold_apple));
    position.y = rnd(0, SCREEN_HEIGHT - 200 - bitmap_width(gold_apple));
    appear = false;
    appear_time = current_time + 1000 + rnd(2000);
    escape_time = 0;
    target_shape = gold_apple;
    expired = false;
}

/** 
 * Draws the golden apple on the screen
 */
void GoldenApple::draw()
{
    drawing_options scale = option_scale_bmp(0.5, 0.5);
    if(appear)
    {
        draw_bitmap(gold_apple, position.x, position.y, scale);
    }
}

/** 
 * Manages the collision of the golden apple, updating score/hearts and creating popup
 * 
 * @param player The player data for updating
 * @param game The current game data
 */
void GoldenApple::hit(player_data &player, game_data &game)
{
    play_sound_effect(gold_hit);
    if(game.state == BONUS_GAME)
    {
        game.bonus_game.multiplier += 1;
        int score = BONUS_SCORE * game.bonus_game.multiplier;
        game.single_player.score += score;
        string text = "+" + to_string(score);
        game.popups.push_back(score_popup{position, text, COLOR_GOLD, current_ticks()}); //Score popup
        //Multiplier popup
        game.popups.push_back(score_popup{position.x,position.y + 20, "x"+ to_string(game.bonus_game.multiplier), COLOR_LIME_GREEN, current_ticks()});
    }
    else
    {
        if (player.hearts < MAX_HEARTS)
        {
            player.hearts++;
            game.popups.push_back(score_popup{position, "+1 Heart", COLOR_GOLD, current_ticks()});
            
        }
        else
        {
            player.score += BONUS_SCORE;
            game.popups.push_back(score_popup{position, "+ 5", COLOR_GOLD, current_ticks()});
        }
    }

    appear = false;
    
}

/** 
 * Creates a new Bomb with a random position
 * 
 * @param current_time The time passed after the timer starts, used to calculate appear time
 */
Bomb::Bomb(long current_time)
{
    position.x = rnd(0, SCREEN_WIDTH - bitmap_width(bomb_shape));
    position.y = rnd(0, SCREEN_HEIGHT - 200 - bitmap_width(bomb_shape));
    appear = false;
    appear_time = current_time + 1000 + rnd(2000);
    escape_time = 0;
    target_shape = bomb_shape;
    expired = false;
}

/**
 * Draws the bomb on the screen
 */
void Bomb::draw()
{
    drawing_options scale = option_scale_bmp(0.5, 0.5);
    if(appear)
    {
        draw_bitmap(bomb_shape, position.x, position.y, scale);
    }

}

/** 
 * Manages the collision of the bomb, updating hearts and creating popup
 * 
 * @param player The player data for updating
 * @param game The current game data
 */
void Bomb::hit(player_data &player, game_data &game)
{
    play_sound_effect(bomb_hit);
    if(game.state == BONUS_GAME)
    {
        game.bonus_game.multiplier = 0;
        game.popups.push_back(score_popup{position.x,position.y, "x0", COLOR_RED, current_ticks()});
    }
    else
    {
        game.popups.push_back(score_popup{position, "-1 Heart", COLOR_RED, current_ticks()});
        player.hearts--;
    }
    appear = false;
    
}

