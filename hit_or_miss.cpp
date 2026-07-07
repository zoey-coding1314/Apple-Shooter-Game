#include "hit_or_miss.h"
#include "global.h"
#include "hom_draw.h"
#include "hom_update.h"
#include "fstream"

using std::to_string;
using namespace std;

/**
 * Save the score of the player after game over into the score.txt file.
 *
 * @param new_score The new score to be saved
 */
void save_score(int new_score)
{
    ofstream write_file("score.txt", ios::app); // Appending new value, not rewrite
    if (!write_file.is_open())
    {
        return;
    }
    else
    {
        write_file << "Score," << new_score << "\n";
    }
    write_file.close();
}

/**
 * Get the best score from the score record in the score.txt file.
 *
 * @return The best score as an integer
 */
int get_best_score()
{
    ifstream file("score.txt");
    int best_score = 0;

    if (!file.is_open())
    {
        return 0; // no file yet → no score
    }

    string line;
    while (getline(file, line)) // Read every line
    {
        // find_last_of(','): return the last comma index: the string with each character has a index that starts from 0.
        int last_comma = line.find_last_of(',');
        if (last_comma != string::npos) // string::npos means "Position not found"
        {
            string score_str = line.substr(last_comma + 1); // Get the string after last comma, start from(last_comma+1) index
            int score = stoi(score_str);

            if (score > best_score)
            {
                best_score = score;
            }
        }
    }

    file.close();
    return best_score;
}

/**
 *  Plays sound effect once per transition, controled by state_sound.
 *
 * @param game The current game data
 */
void play_state_sound(game_data &game)
{
    if (!game.state_sound)
    {
        play_sound_effect(game_over);
        game.state_sound = true;
    }
}

/**
 * Start a bonus streak-based game with initial settings
 *
 * @param bonus The bonus game data
 */
void start_bonus_game(bonus_game_data &bonus)
{
    bonus.basket.x = SCREEN_WIDTH / 2 - bitmap_width(basket_shape) / 2;
    bonus.basket.y = SCREEN_HEIGHT - 110 - bitmap_height(basket_shape);
    bonus.multiplier = 0;
    bonus.last_time = current_ticks();
}

/**
 * Starts a new game with initial settings
 *
 * @param game The current game data
 */
void start_game(game_data &game)
{
    game.state = MENU;
    game.game_timer = create_timer("Game Timer");
    start_timer(game.game_timer);
    game.last_time = timer_ticks(game.game_timer);

    player_data *player = &game.single_player;

    player->score = 0;
    player->streak = 0;
    player->hearts = MAX_HEARTS;
    player->spawn_x = SCREEN_WIDTH / 2 - bitmap_width(arrow_shape) / 2;

    player->arrow = new_arrow();
    player->arrow.position.x = game.single_player.spawn_x;
    player->last_time = timer_ticks(game.game_timer);

    for (int i = 0; i < MAX_TARGET; i++)
    {
        create_random_target(game, i);
    }

    game.state_sound = false;

    fade_music_in(background_music, -1, 2000);
}

/**
 * Start a new multiplayer game with correct settings
 *
 * @param game The current game data
 */
void start_multiplayer(game_data &game)
{
    game.state = MULTIPLAYER;

    game.game_timer = create_timer("Game Timer");
    start_timer(game.game_timer);

    for (int i = 0; i < 2; i++)
    {
        game.players[i].hearts = MAX_HEARTS;
        game.players[i].score = 0;
        game.players[i].streak = 0;
        game.players[i].arrow = new_arrow();
        game.players[i].last_time = timer_ticks(game.game_timer);
    }

    game.players[0].arrow.position.x = SCREEN_WIDTH / 4 - bitmap_width(arrow_shape) / 2; //Initial x position of player1's arrow
    game.players[0].spawn_x = game.players[0].arrow.position.x; //Set as spawn_x, every arrow spawned later will refer to this variable
    game.players[1].arrow.position.x = 3 * SCREEN_WIDTH / 4 - bitmap_width(arrow_shape) / 2;
    game.players[1].spawn_x = game.players[1].arrow.position.x;

    for (int i = 0; i < MAX_TARGET; i++)
    {
        create_random_target(game, i);
    }

    game.state_sound = false;
}

/**
 * Checks if the mouse is clicked on the button
 *
 * @param botton_x The x coordinate of the button
 * @param botton_y The y coordinate of the button
 * @param botton_width The width of the button
 * @param botton_height The height of the button
 * @return true if the button is clicked, otherwise false
 */
bool mouse_clicked_button(int botton_x, int botton_y, int botton_width, int botton_height)
{
    if (point_in_rectangle(mouse_x(), mouse_y(), botton_x, botton_y, botton_width, botton_height))
    {
        return true;
    }
    else if (bitmap_point_collision(pause_image, botton_x, botton_y, mouse_x(), mouse_y()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Handles user input for the game of different states
 *
 * @param game The current game data
 */
void handle_input(game_data &game)
{
    if (game.state == MENU)
    {
        if (mouse_clicked(LEFT_BUTTON) && mouse_clicked_button(BUTTON_X - 200, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT))
        {
            play_sound_effect(tap_sound);
            game.state = PLAYING;
        }
        else if (mouse_clicked(LEFT_BUTTON) && mouse_clicked_button(BUTTON_X + 200, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT))
        {
            play_sound_effect(tap_sound);
            start_multiplayer(game);
            game.state = MULTIPLAYER;
        }
    }
    else if (game.state == PLAYING)
    {
        if (key_typed(ESCAPE_KEY))
        {
            game.single_player.hearts = 0;
        }

        if (key_typed(SPACE_KEY) && !game.single_player.arrow.loose)
        {
            play_sound_effect(loose_sound);
            game.single_player.arrow.loose = true;
        }

        if (mouse_clicked(LEFT_BUTTON) && mouse_clicked_button(SCREEN_WIDTH - 50, 30, bitmap_height(pause_image), bitmap_width(pause_image)))
        {
            play_sound_effect(tap_sound);
            play_sound_effect(pause_sound);
            pause_music();
            game.state = PAUSE;
            game.state_sound = false;
            pause_timer(game.single_player.arrow.aim_timer);
        }

        if (key_typed(B_KEY))
        {
            game.state = BONUS_GAME;
            start_bonus_game(game.bonus_game);
            game.last_time = timer_ticks(game.game_timer);
            game.bonus_game.last_time = timer_ticks(game.game_timer);

            for (int i = 0; i < MAX_TARGET; i++)
            {
                create_bonus_target(game, i);
            }
        }
    }
    else if (game.state == BONUS_GAME)
    {
        if (key_typed(ESCAPE_KEY))
        {
            game.single_player.hearts = 0;
        }

        if (key_down(LEFT_KEY))
        {
            game.bonus_game.basket.x -= 10;
        }

        if (key_down(RIGHT_KEY))
        {
            game.bonus_game.basket.x += 10;
        }

        if (mouse_clicked(LEFT_BUTTON) && mouse_clicked_button(SCREEN_WIDTH - 50, SCREEN_HEIGHT - 60, bitmap_width(pause_image), bitmap_height(pause_image)))
        {
            play_sound_effect(tap_sound);
            play_sound_effect(pause_sound);
            pause_music();
            game.state = PAUSE;
            game.state_sound = false;
            pause_timer(game.single_player.arrow.aim_timer);
        }
    }
    else if (game.state == MULTIPLAYER)
    {
        if (key_typed(ESCAPE_KEY))
        {
            game.players[0].hearts = 0;
            game.players[1].hearts = 0;
        }

        if (key_typed(Z_KEY) && !game.players[0].arrow.loose)
        {
            play_sound_effect(loose_sound);
            game.players[0].arrow.loose = true;
        }

        if (key_typed(M_KEY) && !game.players[1].arrow.loose)
        {
            play_sound_effect(loose_sound);
            game.players[1].arrow.loose = true;
        }

        if (mouse_clicked(LEFT_BUTTON) && mouse_clicked_button(SCREEN_WIDTH - 50, SCREEN_HEIGHT - 60, bitmap_width(pause_image), bitmap_height(pause_image)))
        {
            play_sound_effect(tap_sound);
            play_sound_effect(pause_sound);
            pause_music();
            game.state = MULTIPLAYER_PAUSE;
            game.state_sound = false;
            pause_timer(game.players[0].arrow.aim_timer);
            pause_timer(game.players[1].arrow.aim_timer);
        }
    }
    else if (game.state == PAUSE)
    {
        if (mouse_clicked(LEFT_BUTTON) && mouse_clicked_button(BUTTON_X - 40, BUTTON_Y, BUTTON_WIDTH + 80, BUTTON_HEIGHT))
        {
            play_sound_effect(tap_sound);
            resume_music();
            game.state = PLAYING;
            game.state_sound = false;
            game.single_player.last_time = timer_ticks(game.game_timer);
            resume_timer(game.single_player.arrow.aim_timer);
        }
    }
    else if (game.state == MULTIPLAYER_PAUSE)
    {
        if (mouse_clicked(LEFT_BUTTON) && mouse_clicked_button(BUTTON_X - 40, BUTTON_Y, BUTTON_WIDTH + 80, BUTTON_HEIGHT))
        {
            play_sound_effect(tap_sound);
            resume_music();
            game.state = MULTIPLAYER;
            game.state_sound = false;
            game.players[0].last_time = timer_ticks(game.game_timer);
            game.players[1].last_time = timer_ticks(game.game_timer);
            resume_timer(game.players[0].arrow.aim_timer);
            resume_timer(game.players[1].arrow.aim_timer);
        }
    }
    else if (game.state == GAME_OVER)
    {
        if (mouse_clicked(LEFT_BUTTON) && mouse_clicked_button(BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT))
        {
            play_sound_effect(tap_sound);
            start_game(game);
        }
    }
    else if (game.state == MULTIPLAYER_GAME_OVER)
    {
        if (mouse_clicked(LEFT_BUTTON) && mouse_clicked_button(BUTTON_X, BUTTON_Y + 40, BUTTON_WIDTH, BUTTON_HEIGHT))
        {
            play_sound_effect(tap_sound);
            start_game(game);
        }
    }
}

/**
 * Draws the current game state
 *
 * @param game The current game data
 */
void draw_game(const game_data &game)
{

    switch (game.state)
    {
    case MENU:
        draw_menu(game);
        break;
    case PLAYING:
        draw_play(game);
        break;
    case BONUS_GAME:
        draw_bonus_game(game);
        break;
    case MULTIPLAYER:
        draw_multiplayer(game);
        break;
    case PAUSE:
        draw_pause(game);
        break;
    case MULTIPLAYER_PAUSE:
        draw_multiplayer_pause(game);
        break;
    case GAME_OVER:
        draw_game_over(game);
        break;
    case MULTIPLAYER_GAME_OVER:
        draw_multiplayer_game_over(game);
        break;
    }

    refresh_screen(60);
}

/**
 * Check if the target is catched by the basket
 *
 * @param basket The basket position
 * @param Target The current target data
 */
bool basket_catch_target(point_2d basket, Target &target)
{
    rectangle rect = rectangle_from(basket, bitmap_width(basket_shape), bitmap_height(basket_shape)); //Illustrate the basket as a rectangle
    //Illustrate the targets as circles
    circle target_circle = circle_at(target.get_x() + (bitmap_width(target.get_shape()) / 2), target.get_y() + (bitmap_height(target.get_shape()) / 2), 25);
    return (target.is_appearing() && rectangle_circle_intersect(rect, target_circle));
}

/**
 * Updates the game state, targets, arrow, and popups
 *
 * @param game The current game data
 */
void update_game(game_data &game)
{

    if (game.state == BONUS_GAME)
    {
        long current_time = timer_ticks(game.game_timer);
        double bonus_time = (current_time - game.last_time) / 1000.0;
        double second = (current_time - game.bonus_game.last_time) / 1000.0;
        game.bonus_game.last_time = current_time;

        if (bonus_time > BONUS_LIMIT)//Check if the bonus game has ran passed 10 seconds
        {
            game.state = PLAYING;
            // Recreate all the target for playing state
            for (int i = 0; i < MAX_TARGET; i++)
            {
                create_random_target(game, i);
            }

            game.single_player.streak = 0;
        }
        else
        {
            for (int i = 0; i < MAX_TARGET; i++)
            {
                if (game.Targets[i] != nullptr)
                {
                    game.Targets[i]->bonus_update(second);
                    if (basket_catch_target(game.bonus_game.basket, *game.Targets[i]))
                    {
                        game.Targets[i]->hit(game.single_player, game);
                        recreate_target(game, i);
                    }

                    if (game.Targets[i]->is_expired())
                    {
                        recreate_target(game, i);
                    }
                }
            }
            update_popups(game, current_ticks());
        }

        if (game.single_player.hearts == 0)//This will happen only the player choose to 'esc' the game
        {
            game.state = GAME_OVER;
            save_score(game.single_player.score);
            pause_music();
            play_state_sound(game);
        }
    }
    else
    {
        for (int i = 0; i < MAX_TARGET; i++)
        {
            if (game.Targets[i] != nullptr)
            {

                game.Targets[i]->update(timer_ticks(game.game_timer));

                if (game.Targets[i]->is_expired())
                {
                    recreate_target(game, i);
                }
            }
        }

        if (game.state == PLAYING)
        {
            if (game.single_player.hearts > 0)
            {
                update_arrow(game.single_player, game);
                update_popups(game, current_ticks());

                if (game.single_player.streak == BONUS_TRIGGER)//Consecutive 3 hits will trigger bonus game
                {
                    game.state = BONUS_GAME;
                    start_bonus_game(game.bonus_game);
                    game.last_time = timer_ticks(game.game_timer);
                    game.bonus_game.last_time = timer_ticks(game.game_timer);

                    for (int i = 0; i < MAX_TARGET; i++)
                    {
                        create_bonus_target(game, i);
                    }
                }
            }
            else
            {
                game.state = GAME_OVER;
                save_score(game.single_player.score);
                pause_music();
                play_state_sound(game);
            }
        }

        if (game.state == MULTIPLAYER)
        {
            if (game.players[0].hearts > 0 && game.players[1].hearts > 0)
            {
                update_arrow(game.players[0], game);
                update_arrow(game.players[1], game);
                update_popups(game, current_ticks());
            }
            else
            {
                game.state = MULTIPLAYER_GAME_OVER;
                if (game.players[0].hearts <= 0 || game.players[1].hearts <= 0)
                {
                    if (game.players[0].score > game.players[1].score)
                    {
                        game.winner = 1;
                    }
                    else if (game.players[0].score < game.players[1].score)
                    {
                        game.winner = 2;
                    }
                    else // Draw in scores, refer to number of hearts
                    {
                        if (game.players[0].hearts == 0)
                        {
                            game.winner = 2;
                        }
                        else
                        {
                            game.winner = 1;
                        }
                    }
                }
                save_score(game.players[0].score);
                save_score(game.players[1].score);
                pause_music();
                play_state_sound(game);
            }
        }
    }
}
