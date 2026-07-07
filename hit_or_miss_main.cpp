#include "splashkit.h"
#include "hit_or_miss.h"
#include "global.h"

int main()
{
    open_window("Apple Shooter", SCREEN_WIDTH, SCREEN_HEIGHT);
    game_data game;
    start_game(game);
    
    while (!quit_requested())
    {
        process_events();
        handle_input(game);
        if(game.state != PAUSE && game.state != MULTIPLAYER_PAUSE)
        {
            update_game(game);
        }
        
        draw_game(game);
    }
    return 0;
}