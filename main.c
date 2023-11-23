#include <time.h>

#include "game.h"
#include "debugmalloc.h"


int main(int argc, char *argv[]) {
    (void)argc, (void)argv;
    srand(time(0));

    if (!InitializeSDL()) return 1;

    GameStruct game_struct = GameStruct_Init();
    if (!GameStruct_Create(&game_struct)) {
        QuitSDL();
        return 2;
    }

    StartMusic(game_struct.assets);
    
    int ret = 0;
    if (!Menu(game_struct))
        ret = 3;

    Mix_FadeOutMusic(500);
    GameStruct_Destroy(&game_struct);
    QuitSDL();

    return ret;
}