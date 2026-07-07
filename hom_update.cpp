#include "splashkit.h"
#include "hit_or_miss.h"
#include "global.h"

/**
 * Creates a new arrow with initial position and angle
 *
 * @return A new arrow_data struct with default settings
 */
arrow_data new_arrow()
{
    arrow_data arrow;
    //Arrow position x is set later based on playing mode as single player and multiplayer modes require different position x.
    arrow.position.y = SCREEN_HEIGHT - 80 - bitmap_height(arrow_shape);
    arrow.angle = 0;
    arrow.direction = -1;
    arrow.loose = false;
    arrow.arrow_appear = true;

    arrow.aim_timer = create_timer("Aim Timer");
    start_timer(arrow.aim_timer);
    arrow.aim_time = 0;
    return arrow;
}

/**
 * Creates a random target (Apple, Golden Apple, or Bomb)
 *
 * @param game The current game data
 * @param i The index in the Targets array to create the target
 */
void create_random_target(game_data &game, int i)
{
    int chance = rnd(10);
    if (chance == 0) // 10% chance to be a golden apple
    {
        game.Targets[i] = new GoldenApple(timer_ticks(game.game_timer));
    }
    else if (chance == 1 || chance == 2) // 20% chance to be a bomb
    {
        game.Targets[i] = new Bomb(timer_ticks(game.game_timer));
    }
    else
    {
        game.Targets[i] = new Apple(timer_ticks(game.game_timer));
    }
}

/**
 * Creates a random bonus target(Golden Apple, Bomb)
 *
 * @param game The current game data
 * @param i The index in the Targets array to create the target
 */
void create_bonus_target(game_data &game, int i)
{
    if (rnd(10) == 1 || rnd(10) == 2) // 20% chance to be a bomb
    {
        game.Targets[i] = new Bomb(timer_ticks(game.game_timer));
    }
    else
    {
        game.Targets[i] = new GoldenApple(timer_ticks(game.game_timer));
    }
    game.Targets[i]->initial(); //Change the position y to the top and make it appear
}

/**
 * Recreates a target after it has been hit after deleting the previous one
 *
 * @param game The current game data
 * @param i The index in the Targets array to recreate the target
 */
void recreate_target(game_data &game, int i)
{
    delete game.Targets[i];
    game.Targets[i] = nullptr;

    if (game.state == BONUS_GAME)
    {
        create_bonus_target(game, i);
    }
    else
    {
        create_random_target(game, i);
    }
}

/**
 * Rotates the arrow in the selected range (-90 to 90 degrees)
 *
 * @param arrow The current arrow data
 * @param seconds The time since last update
 */
void arrow_rotating(arrow_data &arrow, double seconds)
{
    arrow.aim_time = timer_ticks(arrow.aim_timer);
    if (arrow.aim_time > 10000) //If the arrow does not shoot for 10 seconds, rotation speed is slowed down
    {
        arrow.angle += arrow.direction * SLOW_ROTATING_SPEED * seconds;
    }
    else
    {
        arrow.angle += arrow.direction * NORMAL_ROTATING_SPEED * seconds;
    }

    if (arrow.angle <= MIN_ANGLE)
    {
        arrow.angle = MIN_ANGLE;
        arrow.direction = 1;
    }
    else if (arrow.angle >= MAX_ANGLE)
    {
        arrow.angle = MAX_ANGLE;
        arrow.direction = -1;
    }
}

/**
 * Checks if the arrow tip hits the apple
 *
 * @param arrow The current arrow data
 * @param target The current target data
 * @param target_shape The bitmap shape of the target
 * @return true if the arrow hits the target, otherwise false
 */
bool arrow_hit_target(const arrow_data &arrow, const Target &target, bitmap target_shape)
{
    point_2d tip;
    double tail_x = arrow.position.x + bitmap_width(arrow_shape) / 2;
    double tail_y = arrow.position.y + bitmap_height(arrow_shape);
    tip.x = tail_x + sine(arrow.angle) * bitmap_height(arrow_shape);
    tip.y = tail_y - cosine(arrow.angle) * bitmap_height(arrow_shape);

    return target.is_appearing() && point_in_circle(tip.x, tip.y, (target.get_x() + bitmap_width(target_shape) / 2), (target.get_y() + bitmap_height(target_shape) / 2), 30);
}

/**
 * Updates the arrow's position and state, checks for collisions with targets
 *
 * @param game The current game data
 */
void update_arrow(player_data &player, game_data &game)
{
    long current_time = timer_ticks(game.game_timer);
    double seconds = (current_time - player.last_time) / 1000.0;
    player.last_time = current_time;

    if (player.arrow.arrow_appear)
    {
        if (!player.arrow.loose)
        {
            arrow_rotating(player.arrow, seconds);
        }
        else if (player.arrow.loose)
        {
            if (player.arrow.position.x < SCREEN_WIDTH + 50 && player.arrow.position.y < SCREEN_HEIGHT + 50 && player.arrow.position.x > -50 && player.arrow.position.y > -50)
            {
                //Calculate the next point for arrow to visualise straight line movement based on the current angle
                double degree = player.arrow.angle;
                double adjacent = sine(degree) * ARROW_SPEED * seconds;
                double opposite = cosine(degree) * ARROW_SPEED * seconds;
                player.arrow.position.x += adjacent;
                player.arrow.position.y -= opposite;

                for (int i = 0; i < MAX_TARGET; i++)
                {
                    if (game.Targets[i]->is_appearing() && arrow_hit_target(player.arrow, *game.Targets[i], game.Targets[i]->get_shape()))
                    {
                        game.Targets[i]->hit(player, game);

                        if (game.Targets[i]->get_shape() == bomb_shape)
                        {
                            player.streak = 0; //Reset streak if a bomb is hit
                        }
                        else
                        {
                            player.streak += 1; //Streak is counted only the target hit is not a bomb
                        }

                        recreate_target(game, i);
                        player.arrow = new_arrow();
                        player.arrow.position.x = player.spawn_x;
                        break;
                    }
                }
            }
            else
            {
                player.arrow = new_arrow();
                player.arrow.position.x = player.spawn_x;
                player.hearts--;
                player.streak = 0; //Reset streak if missed
            }
        }
    }
}

/**
 * Updates and removes score popups after 1 second
 *
 * @param game The current game data
 */
void update_popups(game_data &game, long current_time)
{
    for (int i = 0; i < game.popups.size(); i++)
    {
        if (current_time - game.popups[i].start_time > 1000) // Popup lasts for 1 second
        {
            game.popups.erase(game.popups.begin() + i); // Remove popup, erase does not accept index but iterator(a pointer-like)
            i--;                                        // Adjust index after erasing
        }
    }
}