#include "catch_amalgamated.hpp"
#include "hit_or_miss.h"
#include "hom_update.h"

using Catch::Approx;

/*==================== Create new Arrow ==========================*/
TEST_CASE("New arrow created with correct details and orientation.")
{
    arrow_data arrow = new_arrow();

    REQUIRE(arrow.loose == false);
    REQUIRE(arrow.arrow_appear == true);
    REQUIRE(arrow.angle == 0);
}

/*==================== Starting different types of game mode ======================*/
TEST_CASE("A new game started with required state")
{
    game_data game;
    start_game(game);

    REQUIRE(game.single_player.hearts == MAX_HEARTS);
    REQUIRE(game.state == MENU);
    REQUIRE(game.single_player.score == 0);
    REQUIRE(game.single_player.arrow.loose == false);
    REQUIRE(game.single_player.arrow.arrow_appear == true);
    REQUIRE(game.state_sound == false);
    REQUIRE(music_playing() == true);
    
    for (int i = 0; i < MAX_TARGET; i++)
    {
        REQUIRE(game.Targets[i] != nullptr);
    }

}

TEST_CASE("A new multiplayer game started with initial settings")
{
    game_data game;
    start_multiplayer(game);

    REQUIRE(game.state == MULTIPLAYER);
    REQUIRE(game.players[0].hearts == MAX_HEARTS);
    REQUIRE(game.players[1].hearts == MAX_HEARTS);
    REQUIRE(game.players[0].score == 0);
    REQUIRE(game.players[1].score == 0);
    REQUIRE(game.players[0].arrow.position.x == SCREEN_WIDTH / 4 - bitmap_width(arrow_shape) / 2);
    REQUIRE(game.players[0].spawn_x == game.players[0].arrow.position.x);
    REQUIRE(game.players[1].arrow.position.x == 3 * SCREEN_WIDTH / 4 - bitmap_width(arrow_shape) / 2);
    REQUIRE(game.players[1].spawn_x == game.players[1].arrow.position.x);

    REQUIRE(game.state_sound == false);
    
    for (int i = 0; i < MAX_TARGET; i++)
    {
        REQUIRE(game.Targets[i] != nullptr);
    }
}

TEST_CASE("A new bonus game started with required settings")
{
    game_data game;
    start_bonus_game(game.bonus_game);

    REQUIRE(game.bonus_game.basket.x == SCREEN_WIDTH / 2 - bitmap_width(basket_shape) / 2);
    REQUIRE(game.bonus_game.basket.y == SCREEN_HEIGHT - 110 - bitmap_height(basket_shape) );
    REQUIRE(game.bonus_game.multiplier == 0);
    
}

/*==================== Create different types of targets ======================*/
TEST_CASE("New apple created with valid details")
{
    Target *apple = new Apple();

    REQUIRE(apple->is_appearing() == false);
    REQUIRE(apple->get_shape() == apple_shape);
    REQUIRE(apple->is_expired() == false);
}

TEST_CASE("Golden apple is created with time dependency")
{
    long fake_time = 0;
    Target* gold = new GoldenApple(fake_time);

    REQUIRE(gold->is_appearing() == false);
    REQUIRE(gold->get_shape() == gold_apple);
    REQUIRE(gold->is_expired() == false);
}

TEST_CASE("Bomb is created with time dependency")
{
    long fake_time = 0;
    Target* bomb = new Bomb(fake_time);

    REQUIRE(bomb->is_appearing() == false);
    REQUIRE(bomb->get_shape() == bomb_shape);
    REQUIRE(bomb->is_expired() == false);
}

/*==================== Testing arrow rotation mechanisms ======================*/
TEST_CASE("Arrow angle rotate in selected range")
{
    arrow_data arrow{};

    arrow.angle = 45;
    arrow.direction = 1;
    arrow.aim_time = 0;
    arrow.aim_timer = nullptr;

    arrow_rotating(arrow, 1.0); //Simulate 1 second has passed

    REQUIRE(arrow.angle >= MIN_ANGLE);
    REQUIRE(arrow.angle <= MAX_ANGLE);
}

/*==================== Testing Single Player scoring system ======================*/
TEST_CASE("Hits Apples at different zone will results in different points gained.")
{
    game_data game;
    game.single_player.score = 0;
    game.Targets[0] = new Apple(100,100); //Top zone
    game.Targets[0]->hit(game.single_player, game);

    REQUIRE(game.single_player.score == TOP_SCORE);//+ 20 to the score

    game.Targets[1] = new Apple(100,300); //Middle zone
    game.Targets[1]->hit(game.single_player, game);

    REQUIRE(game.single_player.score == TOP_SCORE + MID_SCORE); //+ 15 to the score.

    game.Targets[1] = new Apple(100,500); //Bottom zone
    game.Targets[1]->hit(game.single_player, game);

    REQUIRE(game.single_player.score == TOP_SCORE + MID_SCORE + LOW_SCORE); //+ 10 to the score.
    
}

TEST_CASE("Hits Golden apple will give bonus points when hearts are full")
{
    game_data game;
    long fake_time = 0;
    game.single_player.hearts = MAX_HEARTS;
    game.single_player.score = 0;

    game.Targets[0] = new GoldenApple(fake_time);
    game.Targets[0]->hit(game.single_player, game);

    REQUIRE(game.single_player.score == BONUS_SCORE);
    REQUIRE(game.single_player.hearts == MAX_HEARTS);
    REQUIRE(game.Targets[0]->is_appearing() == false);
    REQUIRE(game.popups.size() > 0);
}

TEST_CASE("Hits Bomb will reduce one of the hearts.")
{
    game_data game;
    long fake_time = 0;
    game.single_player.hearts = MAX_HEARTS;
    game.Targets[0] = new Bomb(fake_time);
    game.Targets[0]->hit(game.single_player, game);

    REQUIRE(game.single_player.hearts == MAX_HEARTS - 1);
    REQUIRE(game.Targets[0]->is_appearing() == false);
    REQUIRE(game.popups.size() > 0);
}

TEST_CASE("Reduced hearts and created new arrow after a missed hit.")
{
    game_data game;
    game.single_player.hearts = MAX_HEARTS;
    game.game_timer = nullptr;
    game.single_player.arrow = new_arrow();
    game.single_player.arrow.loose = true;
    game.single_player.arrow.position.x += (SCREEN_WIDTH + 100);

    update_arrow(game.single_player, game);

    REQUIRE(game.single_player.hearts == MAX_HEARTS - 1);
    REQUIRE(game.single_player.arrow.loose == false);
    REQUIRE(game.single_player.arrow.arrow_appear == true);
    REQUIRE(game.single_player.arrow.angle == 0);
}

/*======================= Multiplayer and Bonus games' Mechanisms Test =======================*/
TEST_CASE("Multiplayer games support independent scoring system for each player")
{
    game_data game;
    start_multiplayer(game);
    game.Targets[0] = new Apple(100,100);

    game.Targets[0]->hit(game.players[0], game);
    REQUIRE(game.players[0].score > 0);
    REQUIRE(game.players[1].score == 0);
}

TEST_CASE("Test Bonus game scoring system")
{
    game_data game;
    long fake_time = 0;
    start_game(game);
    game.state = BONUS_GAME;
    start_bonus_game(game.bonus_game);
    game.Targets[0] = new GoldenApple(fake_time);
    game.Targets[1] = new GoldenApple(fake_time);
    game.Targets[2] = new Bomb(fake_time);

    game.Targets[0]->hit(game.single_player, game);

    REQUIRE(game.bonus_game.multiplier == 1);
    REQUIRE(game.single_player.score == 5);
    REQUIRE(game.popups.size() == 2);

    game.Targets[1]->hit(game.single_player, game);//Consecutive hits

    REQUIRE(game.bonus_game.multiplier == 2);
    REQUIRE(game.single_player.score == 15); //+ 2*5 into player's score
    REQUIRE(game.popups.size() == 4); //Another 2 popups created

    game.Targets[2]->hit(game.single_player, game);//Hit a bomb

    REQUIRE(game.bonus_game.multiplier == 0);
    REQUIRE(game.single_player.score == 15); //Player's score remains
    REQUIRE(game.popups.size() == 5); //1 additional popup is created
}

/*==================== Testing popups system ==========================*/
TEST_CASE("Score pop-up is created after a hit.")
{
    game_data game;
    long fake_time = 0;
    game.game_timer = nullptr;
    game.Targets[0] = new Apple();
    game.Targets[1] = new GoldenApple(fake_time);
    game.Targets[2] = new Bomb(fake_time);
    game.Targets[0]->hit(game.single_player, game);
    game.Targets[1]->hit(game.single_player, game);
    game.Targets[2]->hit(game.single_player, game);

    REQUIRE(game.popups.size() == 3);
}

TEST_CASE("Score pop-up is removed after 1 second.")
{
    game_data game;
    game.game_timer = nullptr;
    score_popup popup{};
    popup.start_time = 0; 
    game.popups.push_back(popup);

    update_popups(game, 1500); // Simulate 1.5 seconds have passed

    REQUIRE(game.popups.size() == 0);
}

