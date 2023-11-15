#include "game.h"
#include "debugmalloc.h"

GameStruct GameStruct_Init(void) {
    return (GameStruct){.window=Window_Init(), .assets=Assets_Init(), .fonts=Fonts_Init()};
}

bool GameStruct_Create(GameStruct * game_struct) {
    Window_Create(&game_struct->window, "Plumber", DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    Assets_Load(&game_struct->assets, game_struct->window.renderer);
    Fonts_Load(&game_struct->fonts);

    return true;
}

void GameStruct_Destroy(GameStruct * game_struct) {
    Fonts_Destroy(&game_struct->fonts);
    Assets_Destroy(&game_struct->assets);
    Window_Destroy(&game_struct->window);
}