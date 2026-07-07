#include "splashkit.h"
#include "hit_or_miss.h"
#include "global.h"

/**
 * Calculates the middle x position for centered text
 *
 * @param text The text to be centered
 * @param text_font The font of the text
 * @param text_size The size of the text
 * @return The x coordinate to start drawing the text for it to be centered
 */
double get_middle_x(string text, font text_font, int text_size)
{
    return SCREEN_WIDTH / 2 - text_width(text, text_font, text_size) / 2;
}

using std::to_string;

/**
 * Draws the arrow on the screen with anchor point for rotation
 *
 * @param game The current game data
 */
void draw_arrow(game_data &game)
{
    double anchor_x = 0;
    double anchor_y = bitmap_height(arrow_shape) / 2;

    if (game.state == MULTIPLAYER)
    {
        draw_bitmap(arrow_shape, game.players[0].arrow.position.x, game.players[0].arrow.position.y, option_rotate_bmp(game.players[0].arrow.angle, anchor_x, anchor_y));
        draw_bitmap(arrow_shape, game.players[1].arrow.position.x, game.players[1].arrow.position.y, option_rotate_bmp(game.players[1].arrow.angle, anchor_x, anchor_y));
        return;
    }
    else
    {
        draw_bitmap(arrow_shape, game.single_player.arrow.position.x, game.single_player.arrow.position.y, option_rotate_bmp(game.single_player.arrow.angle, anchor_x, anchor_y));
    }
}

/**
 * Draws the random apples on the screen with scaling
 *
 * @param game The current game data
 */
void draw_target(game_data &game)
{
    for (int i = 0; i < MAX_TARGET; i++)
    {
        if (game.Targets[i] != nullptr)
        {
            game.Targets[i]->draw();
        }
    }
}

/**
 * Draws the score popups on the screen for 1 second with upward movement
 *
 * @param game The current game data
 */
void draw_popups(const game_data &game)
{
    long current_time = current_ticks();
    for (int i = 0; i < game.popups.size(); i++)
    {
        double seconds = (current_time - game.popups[i].start_time) / 1000.0;
        draw_text(game.popups[i].text, game.popups[i].text_color, FONT2, FONT_SMALL, game.popups[i].position.x, game.popups[i].position.y - seconds * POP_UP_MOVING);
        // Gradually moving upwards 50 pixels per second
    }
}

/**
 * Draws the basket
 *
 * @param game The current game data
 */
void draw_basket(const game_data &game)
{
    draw_bitmap(basket_shape, game.bonus_game.basket.x, game.bonus_game.basket.y);
}

/**
 * Draws the main menu screen
 *
 * @param game The current game data
 */
void draw_menu(game_data game)
{
    drawing_options scale = option_scale_bmp(0.55, 0.55);
    clear_screen(COLOR_LIGHT_YELLOW);
    draw_text("HIT", COLOR_LIME_GREEN, FONT1, FONT_BIG + 30, 210, 100);
    draw_text("OR", COLOR_SADDLE_BROWN, FONT1, FONT_BIG - 25, 430, 170);
    draw_text("MISS", COLOR_CRIMSON, FONT1, FONT_BIG + 30, 510, 230);

    draw_text("BEST SCORE: ", COLOR_MEDIUM_SEA_GREEN, FONT1, FONT_SMALL - 20, get_middle_x("BEST SCORE: ", FONT1, FONT_SMALL - 20), 380);
    draw_text(to_string(get_best_score()), COLOR_MEDIUM_SEA_GREEN, FONT1, FONT_SMALL, get_middle_x(to_string(get_best_score()), FONT1, FONT_SMALL), 410);

    fill_rectangle(COLOR_MEDIUM_SEA_GREEN, BUTTON_X - 200, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    fill_rectangle(COLOR_MEDIUM_SEA_GREEN, BUTTON_X + 200, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    draw_text("PLAY", COLOR_WHITE, FONT1, FONT_SMALL + 20, 205, 500);
    draw_text("MULTI", COLOR_WHITE, FONT1, FONT_SMALL - 10, 650, 490);
    draw_text("PLAYER", COLOR_WHITE, FONT1, FONT_SMALL - 10, 620, 530);

    draw_bitmap(apple_shape, 180, 160);
    draw_bitmap(heart_shape, 602, 210, scale);
}

/** Draws the gameplay screen
 *
 * @param game The current game data
 */
void draw_play(game_data game)
{
    clear_screen(COLOR_LIGHT_CYAN);
    drawing_options scale = option_scale_bmp(0.3, 0.3); // Scale to 20%

    draw_text("SCORE: " + to_string(game.single_player.score), COLOR_MEDIUM_SEA_GREEN, FONT2, FONT_BIG, 30, 10);

    fill_circle(COLOR_SIENNA, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 110, 80);
    fill_circle(COLOR_LIGHT_CYAN, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 110, 65);
    fill_rectangle(COLOR_PERU, 0, SCREEN_HEIGHT - 110, SCREEN_WIDTH, 180);
    draw_line(COLOR_BLACK, 0, LINE1_Y, SCREEN_WIDTH, LINE1_Y);
    draw_line(COLOR_BLACK, 0, LINE2_Y, SCREEN_WIDTH, LINE2_Y);

    draw_text("Press SPACE to shoot", COLOR_WHEAT, FONT2, FONT_SMALL + 10, 30, SCREEN_HEIGHT - 70);
    draw_text("Press 'esc' to quit", COLOR_WHEAT, FONT2, FONT_SMALL + 10, 30, SCREEN_HEIGHT - 40);

    draw_bitmap(pause_image, SCREEN_WIDTH - 50, 30);

    for (int i = 0; i < game.single_player.hearts; i++)
    {
        draw_bitmap(heart_shape, (i * 40), 40, scale);
    }

    draw_target(game);
    draw_arrow(game);
    draw_popups(game);
}

/**
 * Draws the bonus game screen
 *
 * @param game The current game data
 */
void draw_bonus_game(game_data game)
{
    clear_screen(COLOR_MIDNIGHT_BLUE);
    drawing_options scale = option_scale_bmp(0.3, 0.3); // Scale to 20%

    draw_text("SCORE: " + to_string(game.single_player.score), COLOR_MEDIUM_SEA_GREEN, FONT2, FONT_BIG, 30, 10);
    fill_rectangle(COLOR_PERU, 0, SCREEN_HEIGHT - 110, SCREEN_WIDTH, 180);
    draw_text("Press left or right arrow to move basket", COLOR_WHEAT, FONT2, FONT_SMALL + 10, 30, SCREEN_HEIGHT - 70);
    draw_text("Press 'esc' to quit", COLOR_WHEAT, FONT2, FONT_SMALL + 10, 30, SCREEN_HEIGHT - 40);

    draw_bitmap(pause_image, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 60);

    for (int i = 0; i < game.single_player.hearts; i++)
    {
        draw_bitmap(heart_shape, (i * 40), 40, scale);
    }

    draw_target(game);
    draw_basket(game);
    draw_popups(game);
}

/**
 * Draws the multiplayer screen
 *
 * @param game The current game data
 */
void draw_multiplayer(game_data game)
{
    clear_screen(COLOR_LIGHT_CYAN);
    drawing_options scale = option_scale_bmp(0.3, 0.3); // Scale to 20%

    draw_text("SCORE: " + to_string(game.players[0].score), COLOR_MEDIUM_SEA_GREEN, FONT2, FONT_BIG, 30, 10);
    draw_text("SCORE: " + to_string(game.players[1].score), COLOR_MEDIUM_SEA_GREEN, FONT2, FONT_BIG, SCREEN_WIDTH - 190, 10);

    fill_circle(COLOR_SIENNA, SCREEN_WIDTH / 4, SCREEN_HEIGHT - 110, 80);
    fill_circle(COLOR_LIGHT_CYAN, SCREEN_WIDTH / 4, SCREEN_HEIGHT - 110, 65);
    fill_circle(COLOR_SIENNA, 3 * SCREEN_WIDTH / 4, SCREEN_HEIGHT - 110, 80);
    fill_circle(COLOR_LIGHT_CYAN, 3 * SCREEN_WIDTH / 4, SCREEN_HEIGHT - 110, 65);
    fill_rectangle(COLOR_PERU, 0, SCREEN_HEIGHT - 110, SCREEN_WIDTH, 180);
    draw_line(COLOR_BLACK, 0, LINE1_Y, SCREEN_WIDTH, LINE1_Y);
    draw_line(COLOR_BLACK, 0, LINE2_Y, SCREEN_WIDTH, LINE2_Y);

    draw_text("Player 1: Press Z to shoot", COLOR_WHEAT, FONT2, FONT_SMALL + 10, 30, SCREEN_HEIGHT - 70);
    draw_text("Player 2: Press M to shoot", COLOR_WHEAT, FONT2, FONT_SMALL + 10, SCREEN_WIDTH - 400, SCREEN_HEIGHT - 70);
    draw_text("Press 'esc' to quit", COLOR_WHEAT, FONT2, FONT_SMALL + 10, 30, SCREEN_HEIGHT - 40);

    draw_bitmap(pause_image, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 60);

    for (int i = 0; i < game.players[0].hearts; i++)
    {
        draw_bitmap(heart_shape, (i * 40), 40, scale);
    }

    for (int i = 0; i < game.players[1].hearts; i++)
    {
        draw_bitmap(heart_shape, 805 + (i * 40), 40, scale);
    }

    draw_target(game);
    draw_arrow(game);
    draw_popups(game);
}

/**
 * Draws the pause screen
 *
 * @param game The current game data
 */
void draw_pause(game_data game)
{
    clear_screen(COLOR_PEACH_PUFF);

    draw_text("PAUSE", COLOR_SADDLE_BROWN, FONT1, FONT_BIG, get_middle_x("PAUSE", FONT1, FONT_BIG), 120);
    draw_text("SCORE:", COLOR_BLACK, FONT2, FONT_BIG, get_middle_x("SCORE:", FONT2, FONT_BIG), 250);
    draw_text(to_string(game.single_player.score), COLOR_BLACK, FONT1, FONT_SMALL, get_middle_x(to_string(game.single_player.score), FONT1, FONT_SMALL), 310);

    fill_rectangle(COLOR_MEDIUM_SEA_GREEN, BUTTON_X - 40, BUTTON_Y, BUTTON_WIDTH + 80, BUTTON_HEIGHT);
    draw_text("RESUME", COLOR_WHITE, FONT1, FONT_SMALL + 10, get_middle_x("RESUME", FONT1, FONT_SMALL + 10), 505);
}

/**
 * Draws the multiplayer pause screen
 *
 * @param game The current game data
 */
void draw_multiplayer_pause(game_data game)
{
    clear_screen(COLOR_PEACH_PUFF);

    draw_text("PAUSE", COLOR_SADDLE_BROWN, FONT1, FONT_BIG, get_middle_x("PAUSE", FONT1, FONT_BIG), 120);
    draw_text("Player 1 SCORE:", COLOR_BLACK, FONT2, FONT_BIG, get_middle_x("Player 1 SCORE:", FONT2, FONT_BIG) - 200, 250);
    draw_text("Player 2 SCORE:", COLOR_BLACK, FONT2, FONT_BIG, get_middle_x("Player 2 SCORE:", FONT2, FONT_BIG) + 200, 250);
    draw_text(to_string(game.players[0].score), COLOR_BLACK, FONT1, FONT_SMALL, get_middle_x(to_string(game.players[0].score), FONT1, FONT_SMALL) - 200, 310);
    draw_text(to_string(game.players[1].score), COLOR_BLACK, FONT1, FONT_SMALL, get_middle_x(to_string(game.players[1].score), FONT1, FONT_SMALL) + 200, 310);

    fill_rectangle(COLOR_MEDIUM_SEA_GREEN, BUTTON_X - 40, BUTTON_Y, BUTTON_WIDTH + 80, BUTTON_HEIGHT);
    draw_text("RESUME", COLOR_WHITE, FONT1, FONT_SMALL + 10, get_middle_x("RESUME", FONT1, FONT_SMALL + 10), 505);
}

/**
 * Draws the game over screen
 *
 * @param game The current game data
 */
void draw_game_over(game_data game)
{
    clear_screen(COLOR_LIGHT_YELLOW);

    draw_text("GAME", COLOR_CRIMSON, FONT1, FONT_BIG, 220, 120);
    draw_text("OVER", COLOR_SADDLE_BROWN, FONT1, FONT_BIG, 510, 190);
    draw_text("SCORE:", COLOR_ORANGE, FONT2, FONT_BIG, get_middle_x("SCORE:", FONT2, FONT_BIG), 280);
    draw_text(to_string(game.single_player.score), COLOR_ORANGE, FONT1, FONT_SMALL, get_middle_x(to_string(game.single_player.score), FONT1, FONT_SMALL), 340);

    fill_rectangle(COLOR_MEDIUM_SEA_GREEN, BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    draw_text("MENU", COLOR_WHITE, FONT1, FONT_SMALL + 20, get_middle_x("MENU", FONT1, FONT_SMALL + 20), 500);
}

/**
 * Draws the multiplayer game over screen
 *
 * @param game The current game data
 */
void draw_multiplayer_game_over(game_data game)
{
    clear_screen(COLOR_LIGHT_YELLOW);

    draw_text("GAME", COLOR_CRIMSON, FONT1, FONT_BIG, 220, 100);
    draw_text("OVER", COLOR_SADDLE_BROWN, FONT1, FONT_BIG, 510, 170);

    string winner_msg = "Winner: Player " + to_string(game.winner);

    draw_text(winner_msg, COLOR_ORANGE, FONT2, FONT_BIG, get_middle_x(winner_msg, FONT2, FONT_BIG), 280);
    draw_text("Player 1  SCORE:", COLOR_ORANGE, FONT2, FONT_BIG, get_middle_x("Player 1 SCORE:", FONT2, FONT_BIG) - 200, 330);
    draw_text(to_string(game.players[0].score), COLOR_ORANGE, FONT1, FONT_SMALL, get_middle_x(to_string(game.players[0].score), FONT1, FONT_SMALL) - 150, 390);
    draw_text("Player 2  SCORE:", COLOR_ORANGE, FONT2, FONT_BIG, get_middle_x("Player 2 SCORE:", FONT2, FONT_BIG) + 200, 330);
    draw_text(to_string(game.players[1].score), COLOR_ORANGE, FONT1, FONT_SMALL, get_middle_x(to_string(game.players[1].score), FONT1, FONT_SMALL) + 150, 390);

    fill_rectangle(COLOR_MEDIUM_SEA_GREEN, BUTTON_X, BUTTON_Y + 40, BUTTON_WIDTH, BUTTON_HEIGHT);
    draw_text("MENU", COLOR_WHITE, FONT1, FONT_SMALL + 20, get_middle_x("MENU", FONT1, FONT_SMALL + 20), 540);
}