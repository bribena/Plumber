/**
 * @file main.c
 * @brief A main függvényt tartalmazó fájl.
 */

// Ha Windows-on fordítjuk
#ifdef _WIN32 
#include <windows.h>
#endif

#include <time.h> // rand() seed-jéhez

#include "game.h"
#include "debugmalloc.h"

// Ha Windows-on fordítjuk, az ablak ezt a main függvényt hívja meg
#ifdef WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE FreeProcInstance, PWSTR pCmdLine, int nCmdShow) {
    (void)hInstance, (void)FreeProcInstance, (void)pCmdLine, (void)nCmdShow;
	return main(0, NULL);
}
#endif

/**
 * @brief A main függvény.
 * @param argc Parancssori argumentumok száma (nem használt)
 * @param argv Parancsori argumentumok (nem használt)
 * @return
 *  - 0: Ha sikeresen lefutott a program
 *  - 1: Ha nem sikerült inicializálni az SDL-t
 *  - 2: Ha nem sikerült létrehozni a játékhoz szükséges változókat
 *  - 3: Ha a játék futása során történt hiba
 */
// A main függvény
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