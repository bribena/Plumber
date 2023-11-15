#include <time.h>

#include "game.h"
#include "debugmalloc.h"


int main(int argc, char *argv[]) {
    (void)argc, (void)argv;
    srand(time(0));

    InitializeSDL();

    GameStruct game_struct = GameStruct_Init();
    GameStruct_Create(&game_struct);

    Mix_PlayMusic(game_struct.assets.bgm, -1);
    Mix_VolumeMusic(70);
    Menu(game_struct);

    Mix_FadeOutMusic(500);
    GameStruct_Destroy(&game_struct);
    QuitSDL();
    return 0;
}