#include "game.h"
#include "debugmalloc.h"

GameStruct GameStruct_Init(void) {
    return (GameStruct){.window=Window_Init(), .assets=Assets_Init(), .fonts=Fonts_Init()};
}

bool GameStruct_Create(GameStruct * game_struct) {
    if (!Window_Create(&game_struct->window, "Plumber", DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT)) return false;
    if (!Assets_Load(&game_struct->assets, game_struct->window)) {
        Window_Destroy(&game_struct->window);
        return false;
    }

    if (!Fonts_Load(&game_struct->fonts)) {
        Assets_Destroy(&game_struct->assets);
        Window_Destroy(&game_struct->window);
        return false;
    }

    return true;
}

void GameStruct_Destroy(GameStruct * game_struct) {
    Fonts_Destroy(&game_struct->fonts);
    Assets_Destroy(&game_struct->assets);
    Window_Destroy(&game_struct->window);
}