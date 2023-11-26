/** 
 * @file SDL_local.h
 * 
 * @brief Ez a modul tartalmazza az SDL-lel kapcsolatos függvényeket, és a hozzájuk tartozó adatszerkezeteket.
 * 
 * @details
 * Struktúrák:
 * -----------
 * 4 struktúra szerepel benne:
 * - {@link Window}: Az ablakot és a renderert tartalmazza, valamint betöltéskor a méreteket is eltárolja.
 * - {@link Assets}: Az assets mappa tartalmát tárolja. (Csak erre a projektre működik, egyébként meg kell változtatni)
 * - {@link Fonts}: A betűtípusokat tárolja. (Jelenleg csak a bold van használatban)
 * - {@link Layer}: Egy renderelési réteg. A renderelt kép tartalmának csoportosítására szolgál, 
 *          hogy ne kelljen mindent külön-külön kirajzolni.
 * 
 * A struktúrák használata többnyire az alábbi módon történik:
 * @code{.c}
 * Struktúra név = Struktúra_Init(); // Az init egy üres elemet ad vissza, nem foglal magának memóriát
 * Struktúra_LétrehozóFüggvény(&struktúra, paraméterek); // Lásd lentebb
 * 
 * // Használat
 *
 * Struktúra_Destroy(&struktúra); // Mindegyik struktúra tartalmaz dinamikusan foglalt elemet, ezért ezeket fel kell szabadítani
 * @endcode
 * 
 * Függvények:
 * -----------
 * A modul tartalmazza a struktúrákhoz kapcsolódó függvényeket.
 * Ezek mellett tartalmazza az SDL inicializációjához és leállításához szükséges függvényeket.
 * 
 * Az SDL modult a main() függvényben kell inicializálni, majd használat után lezárni.
 * 
 * Példakód:
 * ----------
 * @code{.c}
 * #include "SDL_local.h"
 * 
 * int main(int argc, char * argv[]) {
 *     (void)argc, (void)argv; // Csak a fordító ne kapjon warningot, az SDL-nek kell ez a két paraméter
 *     InitializeSDL(); // SDL Inicializálása
 *             
 *     Window ablak = Window_Init(); // Ablak inicializálása
 *     Window_Create(&ablak, "Példa", 800, 600); // Ablak létrehozása
 *     Assets assets = Assets_Init(); // Assets inicializálása
 *     Assets_Load(&assets, ablak); // Assets betöltése
 *     Fonts fonts = Fonts_Init(); // Betűtípusok inicializálása
 *     Fonts_Load(&fonts); // Betűtípusok betöltése
 *             
 *     Layer background = Layer_Init(); // Háttér réteg inicializálása
 *     Layer_Create(&background, ablak, NULL); // Háttér réteg létrehozása
 *     SDL_SetRenderTarget(ablak.renderer, background.layer); // A háttér rétegét változtatjuk
 *     SDL_RenderCopy(ablak.renderer, assets.background, NULL, NULL); // Háttérkép renderelése
 *             
 *     Layer text = Layer_Init(); // Szöveg réteg inicializálása
 *     Layer_Create(&text, ablak, (SDL_Rect){200, 200, 100, 400}); // Szöveg réteg létrehozása
 *     Layer_RenderFont(&text, ablak, fonts.bold, "Hello World!", (SDL_Color){255, 255, 255, 255}); // Szöveg létrehozása
 *     SDL_RenderCopy(ablak.renderer, text.layer, NULL, &text.location); // Szöveg renderelése
 *     Layer_Destroy(&text); // Használat utáni felszabadítás
 *         
 *     SDL_SetRenderTarget(ablak.renderer, NULL); // Renderer target visszaállítása
 *     SDL_RenderCopy(ablak.renderer, background.layer, NULL, NULL); // A háttér réteg renderelése az ablakra
 *     SDL_RenderPresent(ablak.renderer); // Renderelés megjelenítése
 *     SDL_Delay(2000); // 2 másodpercig megjelenítés
 *        
 *     // ...
 *       
 *     // Használat utáni felszabadítás és kilépés
 *     Layer_Destroy(&background);
 *     Fonts_Destroy(&fonts);
 *     Assets_Destroy(&assets);
 *     Window_Destroy(&ablak);
 *     QuitSDL();
 *     return 0;
 * }
 *     @endcode
 * 
 * A fenti példakód megnyit egy ablakot 2 másodpercig, betölti a háttérképet, majd ráírja a "Hello world!"-öt.
 *
 *  Megjegyzések:
 * --------------
 *  - Minden innen származó struktúrát használat után fel kell szabadítani.
 *  - A Layer, mivel SDL_Texture*-t tárol, kompatibilis az SDL függvényekkel.
 *  - A Layer-ek az SDL_SetRenderTarget() használatához lett kitalálva, ennek a "layer" elemét kell átadni,
 *     hogy változtatgassuk bármilyen SDL_Render-es fügvénnyel. Ha az ablakra akarunk renderelni, mindenképpen vissza kell 
 *     váltani az SDL_SetRenderTarget(renderer, NULL) hívásával.
 *  - A szövegek kiírása lehetséges, viszont "mókolós". Itt egy tetszőleges szélesség lett megadva a text-hez, 
 *     viszont ez teljesen elronthatja az arányokat. Sajnos trial and error-ral lehet csak megadni egy-egy szöveg méreteit.
 *  - A zenelejátszást nem foglaltam bele a példába, ez már csak egy "bónusz" a modulban, ez is lehetséges.
 * 
 * Kapcsolódó kódfájlok:
 * ---------------------
 *  - SDL_local.c
 *  - window.c
 *  - layer.c
 *  - assets.c
 *  - fonts.c
 * 
 * @pre Az InitializeSDL() használata szükséges a modul használatához.
 * @post A QuitSDL() használata szükséges a modul használata után.
 * @bug Az SDL használata miatt memóriaszivárgások előfordulhatnak.
*/

/* SDL_local.h eleje */
#ifndef SDL_LOCAL_H
#define SDL_LOCAL_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL2_gfxPrimitives.h>
#include <stdbool.h>


/**
 * @brief Alapértelmezett betűméret
 * @details Az alapértelmezett "betűméret". A jelenlegi szöveghasználat mellett inkább a betűk minősége állítható vele. 
 * Ezen az értéken megfelelő minőségűek.
*/
#define DEFAULT_FONT_SIZE 200
/**
 * @brief Alapértelmezett hangerő
 * @details A zene hangereje (0-128). A játékon belül nem állítható, viszont, hogy ne legyen túl hangos a zene,
 * muszáj volt lentebb állítani a hangerejét. Tetszés szerint lehet állítani.
*/
#define MIX_VOLUME 70

/**
 * @struct Window
 * @brief Window struktúra
 * @details Az ablakot és a renderert tartalmazza, valamint betöltéskor a méreteket is eltárolja.
 */
typedef struct Window {
    /**
     * @details Az ablak pointere.
     */
    SDL_Window * window;
    /**
     * @details A renderer pointere.
     */
    SDL_Renderer * renderer;
    /**
     * @var width
     * @details Ablakszélesség (pixelben)
     */
    /**
     * @var height
     * @details Ablakmagasság (pixelben)
    */
    int width, height;
} Window;

/**
 * @struct Assets
 * @brief Assets mappa fájlaihoz struktúra
 * @details Az assets mappa tartalmát megfelelő változókban tárolja. (Csak erre a projektre működik,
 * egyébként meg kell változtatni.) Maga az Assets kevés helyen használt, inkább csoportosításra szolgál.
 */
typedef struct Assets {
    /**
     * @details A háttérkép textúrája.
     */
    SDL_Texture * background;
    /**
     * @details A csövek textúrája.
     */
    SDL_Texture * pipes_empty;
    /**
     * @details Szintén csövek textúrája, de egy TextureMod és egy AlphaMod van rárakva, hogy "vízzel teli" hatást adjon neki.
     */
    SDL_Texture * pipes_full;
    /**
     * @details A csap textúrája.
     */
    SDL_Texture * wheel;
    /**
     * @details A háttérzene pointere.
     */
    Mix_Music * bgm;
} Assets;

/**
 * @struct Fonts
 * @brief Betűtípusokhoz struktúra
 * @details A betűtípusokat tárolja. (Jelenleg csak a bold van használatban.) Szövegeket a Layer_RenderFont()-tal
 * lehet létrehozni, ahol a Fonts struktúra egyik elemét lehet átadni.
 */
typedef struct Fonts {
    /**
     * @details A normál betűtípus pointere.
     */
    TTF_Font * normal;
    /**
     * @details A dőlt betűtípus pointere.
     */
    TTF_Font * italic;
    /**
     * @details A félkövér betűtípus pointere. (Jelenleg csak ez van használatban)
     */
    TTF_Font * bold;
    /**
     * @details A félkövér dőlt betűtípus pointere.
     */
    TTF_Font * bold_italic;
} Fonts;

/**
 * @struct Layer
 * @brief Réteg struktúra
 * @details Egy renderelési réteg. A renderelt elemek csoportosítására szolgál, hogy ne kelljen mindent külön-külön kirajzolni.
 * A layer eleme SDL_SetRenderTarget()-tel kiválasztható, ezután az összes renderelés erre hajtódik végre.
 * A location a szülejében elfoglalt helyét és méretét tárolja. Szülőnek a létrehozáskor használt render target-et kell
 * tekinteni.
 */
typedef struct Layer {
    /**
     * @details A réteg textúrája. SDL_SetRenderTarget() által változtatható. 
     */
    SDL_Texture * layer;
    /**
     * @details A réteg helye és mérete a "szülejében".
     */
    SDL_Rect location;
} Layer;

/**
 * @brief Az SDL inicializáló függvénye
 * @details Az SDL-t és a hozzá kapcsolódó modulokat inicializálja. Ha használni akarjuk az SDL-t, akkor ezt a függvényt
 * meg kell hívni.
 * Az előforduló hibákat a függvény kiírja a konzolra.
 * @post A használat után a QuitSDL() függvényt kell meghívni, hogy leállítsuk a különböző modulokat.
 * @warning Többszöri hívás kilépés nélkül összeomlást okozhat.
 * @return true, hiba esetén false
 */
bool InitializeSDL(void);
/**
 * @brief Elindítja a háttérzenét és végtelenül ismétli
 * @details SDL_mixer függvényekkel indítja el a háttérzenét.
 * Ezután az SDL_mixer függvényekkel kezelhető az Assets bgm eleme.
 * Az előforduló hibákat a függvény kiírja a konzolra.
 * @post Bár a QuitSDL() leállítja a zenét, mégis érdemes manuálisan leállítani, kilépés előtt.
 * @param assets Az assets mappa tartalmát tároló struktúra. A bgm eleme kell.
 */
void StartMusic(Assets assets);
/**
 * @brief Leállítja az SDL-lel kapcsolatos modulokat
 * @details Ha az InitializeSDL() meg volt hívva, ezt is meg kell hívni a kilépés előtt. 
 */
void QuitSDL(void);

/**
 * @brief Window struktúra inicializálása (kinullázása)
 * @return Egy üres Window struktúra.
 */
Window Window_Init(void);
/**
 * @brief Feltölt egy inicializált Window struktúrát.
 * @details Létrehoz egy SDL_Window*-ot és egy SDL_Renderer*-t, majd a window paraméterben eltárolja ezeket.
 * Az előforduló hibákat a függvény kiírja a konzolra.
 * Hiba esetén nem ajánlott használni a struktúrát.
 * @post A Window_Create() által létrehozott Window struktúrákat használat után a Window_Destroy()-al kell felszabadítani.
 * @warning TILOS egy, már létrehozott, Window struktúrát átadni, mert memóriaszivárgás lehet belőle.
 * @param window Egy inicializált Window struktúra pointere.
 * @param title Az ablak neve
 * @param width Az ablak szélessége (pixelben)
 * @param height Az ablak magassága (pixelben)
 * @return true, hiba esetén false
 */
bool Window_Create(Window * window, const char * title, int width, int height);
/**
 * @brief Felszabadít egy Window struktúrát
 * @details Az átadott Window_Create() által létrehozott struktúrát szabadítja fel.
 * @param window A felszabadítandó Window struktúra pointere.
 */
void Window_Destroy(Window * window);

/**
 * @brief Assets struktúra inicializálása (kinullázása)
 * @return Egy üres Assets struktúra.
 */
Assets Assets_Init(void);
/**
 * @brief Feltölt egy inicializált Assets struktúrát.
 * @details Betölti az assets mappa tartalmát a megfelelő változókba.
 * Az előforduló hibákat a függvény kiírja a konzolra.
 * Hiba esetén nem ajánlott használni a struktúrát.
 * @post Az Assets_Load() által létrehozott Assets struktúrákat használat után az Assets_Destroy()-al kell felszabadítani.
 * @warning Az Assets struktúrát csak egyszer kell létrehozni, de ha valamiért mégegyszer kell az Assets_Load(), TILOS egy,
 * már létrehozott, Assets struktúrát átadni, mert memóriaszivárgás lehet belőle.
 * @param assets Egy inicializált Assets struktúra pointere.
 * @param window Az renderert tartalmazó Window struktúra.
 * @return true, hiba esetén false
 */
bool Assets_Load(Assets * assets, Window window);
/**
 * @brief Felszabadít egy Assets struktúrát
 * @details Az átadott Assets_Load() által létrehozott struktúrát szabadítja fel.
 * @param assets A felszabadítandó Assets struktúra pointere.
 */
void Assets_Destroy(Assets * assets);

/**
 * @brief Fonts struktúra inicializálása (kinullázása)
 * @return Egy üres Fonts struktúra.
 */
Fonts Fonts_Init(void);
/**
 * @brief Feltölt egy inicializált Fonts struktúrát.
 * @details Betölti a betűtípusokat az assets/fonts mappából a megfelelő változókba.
 * Az előforduló hibákat a függvény kiírja a konzolra.
 * Hiba esetén nem ajánlott használni a struktúrát.
 * A betöltött betűtípusokat a Layer_RenderFont() függvénnyel lehet használni.
 * @post Az Fonts_Load() által létrehozott Fonts struktúrákat használat után az Fonts_Destroy()-al kell felszabadítani.
 * @warning A Fonts struktúrát csak egyszer kell létrohozni, de ha valamiért mégegyszer kell az Fonts_Load(), TILOS egy,
 * már létrehozott, Fonts struktúrát átadni, mert memóriaszivárgás lehet belőle.
 * @param fonts Egy inicializált Fonts struktúra pointere.
 * @return true, hiba esetén false
 */
bool Fonts_Load(Fonts * fonts);
/**
 * @brief Felszabadít egy Fonts struktúrát
 * @details Az átadott Fonts_Load() által létrehozott struktúrát szabadítja fel.
 * @param fonts A felszabadítandó Fonts struktúra pointere.
 */
void Fonts_Destroy(Fonts * fonts);

/**
 * @brief Layer struktúra inicializálása (kinullázása)
 * @return Egy üres Layer struktúra.
 */
Layer Layer_Init(void);
/**
 * @brief Feltölt egy inicializált Layer struktúrát.
 * @details Létrehoz egy SDL_Texture*-t SDL_TEXTUREACCESS_TARGET-tel, majd a Layer struktúra layer elemében eltárolja ezt.
 * Az előforduló hibákat a függvény kiírja a konzolra.
 * Hiba esetén nem ajánlott használni a struktúrát.
 * @post A Layer_Create() által létrehozott Layer struktúrákat használat után a Layer_Destroy()-al kell felszabadítani.
 * @warning TILOS egy, már létrehozott, Layer struktúrát átadni, mert memóriaszivárgás lehet belőle.
 * @param layer Egy inicializált Layer struktúra pointere.
 * @param window A renderert tartalmazó Window struktúra.
 * @param location A réteg helye és mérete a "szülőjéhez" relatívan. Ha NULL, akkor a Window méretét veszi fel! 
 * @return true, hiba esetén false
 */
bool Layer_Create(Layer * layer, Window window, const SDL_Rect * location);
/**
 * @brief Létrehoz egy szöveget tartalmazó Layer-t
 * @details Egy SDL_Surface*-en létrehoz egy szöveget, majd SDL_CreateTextureFromSurface()-el átalakítja SDL_Texture*-ré.
 * Egy Layer_Create() által létrehozott Layer-t kell átadni, viszont valójában ebből csak a location kell. Emiatt
 * technikailag bármilyen Layer-re lehet használni, de a Layer_Create()-el létrehozott Layer-eket célszerű használni.
 * Az előforduló hibákat a függvény kiírja a konzolra.
 * Hiba esetén nem ajánlott használni a struktúrát.
 * @post Használat után a Layer_Destroy()-al kell felszabadítani a léterhozott struktúrát.
 * @warning A függvény nagyon egyszerű, ezért a szöveg szélességét nem számítja ki, ezért előre meg kell adni. Ebből adódik,
 * hogy próbálgatással lehet csak viszonylag megfelelő arányokat beállítani.
 * @param layer Egy Layer_Create()-el létrehozott Layer struktúra pointere.
 * @param window A renderert tartalmazó Window struktúra.
 * @param font A használni kívánt betűtípus.
 * @param text A renderelendő szöveg.
 * @param foreground A betűk színe.
 * @return true, hiba esetén false
 */
bool Layer_RenderFont(Layer * layer, Window window, TTF_Font * font, const char * text, SDL_Color foreground);
/**
 * @brief Felszabadít egy Layer struktúrát
 * @details Az átadott Layer_Create() által létrehozott struktúrát szabadítja fel.
 * @param layer A felszabadítandó Layer struktúra pointere.
 */
void Layer_Destroy(Layer * layer);

#endif
/* SDL_local.h vége */