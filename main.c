/**
 * @file main.c
 * @brief A main függvényt tartalmazó fájl.
 */

#include <time.h> // rand() seed-jéhez

#include "game.h"
#include "debugmalloc.h"


int main(int argc, char *argv[]) {
    (void)argc, (void)argv; // Warning elkerülése
    srand(time(0)); // Random seed

    if (!InitializeSDL()) return 1; // SDL inicializálása

    GameStruct game_struct = GameStruct_Init(); // SDL_local.h változóinak inicializálása
    if (!GameStruct_Create(&game_struct)) { // SDL_local.h változóinak létrehozása
        QuitSDL(); // Hiba esetén kilépés
        return 2;
    }

    StartMusic(game_struct.assets); // Zene indítása
    
    int ret = 0;
    if (!Menu(game_struct)) // Menübe lépés
        ret = 3;

    // Kilépés előtti felszabadítás
    Mix_FadeOutMusic(500);
    GameStruct_Destroy(&game_struct);
    QuitSDL();

    return ret;
}