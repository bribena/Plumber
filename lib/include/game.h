/**
 * @file game.h
 * 
 * @brief A játék headerje.
 * 
 * @details
 * A játtékkal kapcsolatban mindent tartalmaz. Innen már csak a main()-ben kell meghívogatni néhány függvényt.
 * A header tartalmának nagy része csak a sok forrásfájl miatt szerepel itt.
 * 
 * @pre Mivel az SDL_local.h-t használja, az SDL inicializálása szükséges a modul megfelelő működéséhez.
 * 
 * Struktúrák, enumok:
 * -----------
 *  - {@link GameStruct}: Az SDL_local.h-ban szereplő struktúrákat fogja össze.
 *  - {@link EventloopVariables}: A Game() függvény/kép eventloopjának a változóit fogja össze.
 *  - {@link Wheel}: A csaphoz tartozó változókat fogja össze.
 *  - {@link AnimationQueue}: Az felhasználó által indított animációkhoz egy strázsás láncolt lista.
 * 
 * Emellett az animáláshoz 2 enumot is tartalmaz a header:
 *  - {@link AnimationType}: Az animáció típusa.
 *  - {@link AnimationDirection}: Az animáció iránya.
 * 
 * Függvények:
 * -----------
 * Az itt szereplő függvények vagy a játékmenethez kapcsolódnak vagy a nevükben szereplő struktúrákhoz.
 * A játék 3 állapota (menü, játék, nyerés kép) a saját függvényeikbe vannak szervezve.
 * 
 * Példakód:
 * ---------
 * @include main.c
 * 
 * Megjegyzések:
 * -------------
 *  - A projekt main() függvényéből is látszik, hogy csak a GameStruct-ot kell létrehozni és a Menu() függvényt meghívni.
 *    A többi függvény hívást és struktúra létrehozást a Menu() végzi.
 *  - A különböző if-ek a hibakezelés miatt vannak, hogy a program minél kevesebb memóriaszivárgással omoljon össze.
 * 
 * Kapcsolódó kódfájlok:
 * ---------------------
 *  - game.c
 *  - game_struct.c
 *  - ev_vars.c
 *  - wheel.c
 *  - animation.c
 * 
 * @bug A modul egy osdialog nevű modullal hoz létre rendszerfüggő dialógusablakokat. Ez a GTK2-re épül Linux alatt, ami
 * memóriaszivárgást okoz. Windows alatt az Address Sanitizert nem teszteltem, ott nem tudom, hogy mi a helyzet.
 * Egy lehetséges megoldás, hogy az osdialog-ot mással helyettesítjük, de emiatt a game.c-ben változtatásokat kell hozni.
 * 
 */

/* game.h eleje */
#ifndef GAME_H
#define GAME_H

#include "SDL_local.h"
#include "pipe.h"
#include "osdialog.h"

/**
 * @brief Alapértelmezett ablakszélesség.
 * @details Az ablak szélességét határozza meg. A GameStruct_Create() függvény használja a Window struktúra létrehozásakor.
 */
#define DEFAULT_WINDOW_WIDTH 1600
/**
 * @brief Alapértelmezett ablakmagasság.
 * @details Az ablak magasságát határozza meg. A GameStruct_Create() függvény használja a Window struktúra létrehozásakor.
 */
#define DEFAULT_WINDOW_HEIGHT 900

/**
 * @brief A játék pályáján szereplő, egy sorban lévő csövek minimális száma.
 * @details Egy sorban legalább ennyi cső szerepel.
 */
#define MIN_X_SIZE 18
/**
 * @brief A játék pályáján szereplő, egy oszlopban lévő csövek minimális száma.
 * @details Egy oszlopban legalább ennyi cső szerepel.
 */
#define MIN_Y_SIZE 14

/**
 * @brief A csap csövekhez viszonyított mérete.
 * @details Ennyiszer nagyobb egy csőhöz képest.
 */
#define WHEEL_SIZE_MULTIPLIER 1.75

/**
 * @brief A csap forgatási animációjának a sebessége.
 * @details Más animációja nincs a csapnak.
 */
#define WHEEL_ROTATE_SPEED 2
/**
 * @brief A csövek forgatási animációjának a sebessége.
 */
#define PIPE_ROTATE_SPEED 3
/**
 * @brief A pálya előtt lévő hosszú cső feltöltési animációjának a sebessége.
 */
#define LONG_PIPE_FILL_SPEED 10
/**
 * @brief A pályán lévő csövek feltölési animációjának a sebessége.
 */
#define PIPE_FILL_SPEED 20

/**
 * @enum AnimationType
 * @brief Az AnimationQueue egy elemének típusa.
 * @details Esetszétválasztásra szolgál.
 */
typedef enum AnimationType {
    /**
     * @brief Alapértelmezett érték.
     */
    AnimationType_None,
    /**
     * @brief Forgatás.
     */
    AnimationType_Rotate,
    /**
     * @brief Feltöltés.
     */
    AnimationType_Fill
} AnimationType;

/**
 * @enum AnimationDirection
 * @brief Az AnimationQueue-ban szereplő feltöltési animáció iránya.
 * @details Csak AnimationType_Fill esetén értelmezett.
 */
typedef enum AnimationDirection {
    /**
     * @brief Jobbra tölt.
     */
    AnimationDirection_ToRight,
    /**
     * @brief Balra tölt.
    */
    AnimationDirection_ToLeft,
    /**
     * @brief Felfele tölt.
     */
    AnimationDirection_ToUp,
    /**
     * @brief Lefele tölt.
     */
    AnimationDirection_ToDown
} AnimationDirection;

/**
 * @struct GameStruct
 * @brief A játékhoz szükséges struktúrákat fogja össze.
 * @details Az SDL_local.h struktúráit tartalmazza. Könnyebb kezelhetőség a célja, hiszen mindhárom struktúrát létre kell hozni
 * a megfelelő működés érdekében.
 */
typedef struct GameStruct {
    /**
     * @brief Window struktúra.
     * @details Az ablakkal kapcsolatos változókat tartalmazza.
     */
    Window window;
    /**
     * @brief Assets struktúra.
     * @details Az assets mappa tartalmát fogja össze.
     */
    Assets assets;
    /**
     * @brief Fonts struktúra.
     * @details Az assets/fonts mappában található betűtípusokat tartalmazza.
     */
    Fonts fonts;
} GameStruct;

/**
 * @struct EventloopVariables
 * @brief A játék eventloopjának a változóit fogja össze.
 * @details A Game() függvényben használt.
 */
typedef struct EventloopVariables {
    /**
     * @brief A bejövő event.
     * @details Az SDL_PollEvent() vagy SDL_WaitEvent() függvényeknek lehet átadni.
     */
    SDL_Event event;
    /**
     * @brief A játék állapota.
     * @details Ha true, az eventloop fut, egyébként false.
     */
    bool running;
    /**
     * @brief Kattintás állapota.
     * @details true, amíg nem érkezik SDL_MOUSEBUTTONUP event.
     */
    bool clicked;
    /**
     * @brief Az SDL_MOUSEBUTTONDOWN event érkezésekor kimentett kurzorpozíció.
     */
    SDL_Point prevCursor;
    /**
     * @brief Az SDL_GetPerformanceCounter() függvény értéke.
     */
    Uint64 currTime;
    /**
     * @brief Az SDL_GetPerformanceCounter() függvény előző értéke.
     */
    Uint64 prevTime;
    /**
     * @brief A két idő közötti különbség az SDL_GetPerformanceFrequency() értékével elosztva. 
     */
    double deltaT;
} EventloopVariables;

/**
 * @struct Wheel
 * @brief A csap struktúrája.
 * @details A csap megjelenítéséhez és forgatásához szükséges változókat tartalmazza.
 */
typedef struct Wheel {
    /**
     * @brief A csap Layer-e
     */
    Layer layer;
    /**
     * @brief A csap forgatási szöge (fokban).
     */
    double szog;
} Wheel;

/**
 * @struct AnimationQueue
 * @brief Az animációs sor struktúrája.
 * @details Az animációs sor maga egy strázsás láncolt lista. Kétféle animációhoz kezeléséhez tárol változókat
 */
typedef struct AnimationQueue {
    /**
     * @brief Az animáció állapota.
     * @details true, amíg az animáció nem ért véget.
     */
    bool active;
    /**
     * @brief Az animáció típusa.
     */
    AnimationType type;
    /**
     * @brief Az animációt Pipe-on kell-e végrehajtani.
     */
    bool is_pipe;
    /**
     * @brief Az animáció sebessége.
     */
    double animation_speed;

    /**
     * @brief A Pipe pointere. (Csak is_pipe esetén értelmes)
     */
    Pipe * pipe;
    /**
     * @brief A Wheel pointere. (Csak !is_pipe és AnimationType_Rotate esetén értelmes)
     */
    Wheel * wheel;
    
    /**
     * @brief Meghatározza a forgatás mértékét (fokban). (Csak AnimationType_Rotate esetén értelmes)
     */
    double target_angle;
    /**
     * @brief A struktúra által megtett szög (fokban). (Csak AnimationType_Rotate esetén értelmes)
     */
    double rendered_angle;
    /**
     * @brief A forgatandó elem szögének pointere. (Csak AnimationType_Rotate esetén értelmes)
     */
    double * angle;

    /**
     * @brief Az animáció iránya. (Csak AnimationType_Fill esetén értelmes)
     */
    AnimationDirection direction;
    /**
     * @brief A renderelendő hossz (pixelben). (Csak AnimationType_Fill esetén értelmes)
     */
    double target_length;
    /**
     * @brief A cél koordináta. (Csak AnimationType_Fill esetén értelmes)
     */
    double target_coor;
    /**
     * @brief A render_location és a texture_location aránya. (Csak AnimationType_Fill esetén értelmes)
     */
    double ratio;
    /**
     * @brief A renderelendő textúra helye a filled_texuter-n belül. (Csak AnimationType_Fill esetén értelmes)
     */
    SDL_Rect texture_location;
    /**
     * @brief Az elem RenderTarget-en belüli helye. (Csak AnimationType_Fill esetén értelmes)
     */
    SDL_Rect render_location;
    /**
     * @brief A renderelendő textúra. (Csak AnimationType_Fill esetén értelmes)
     */
    SDL_Texture * filled_texture;

    /**
     * @brief A következő elem pointere.
     */
    struct AnimationQueue * next;
} AnimationQueue;

/**
 * @brief GameStruct inicializálása (kinullázása)
 * @return Egy üres GameStruct
 */
GameStruct GameStruct_Init(void);
/**
 * @brief GameStruct létrehozása
 * @details Meghívja a GameStruct elemeinek létrehozó függvényeit (lásd: SDL_local.h). A Window létrehozásához a 
 * DEFAULT_WINDOW_WIDTH és DEFAULT_WINDOW_HEIGHT konstansokat használja.
 * Átadni egy GameStruct_Init()-tel inicializált GameStruct-ot ajánlott.
 * A hibákat a struktok létrehozó függvényei a konzolra kiírják, ennek csak a visszatérési értéke jelzi.
 * Hiba esetén nem ajánlott a struktúra használata.
 * @post Használat után a GameStruct_Destroy() függvénnyel fel kell szabadítani a struktúrát.
 * @warning TILOS egy már létrehozott GameStruct-ot átadni.
 * @param game_struct Egy inicializált GameStruct pointere.
 * @return true, hiba esetén false
 */
bool GameStruct_Create(GameStruct * game_struct);
/**
 * @brief Felszabadít egy GameStruct-ot.
 * @details A GameStruct elemeit szabadítja fel a saját függvényeikkel. (lásd: SDL_local.h)
 * @param game_struct A felszabadítandó GameStruct pointere.
 */
void GameStruct_Destroy(GameStruct * game_struct);

/**
 * @brief Az eventloop változóinak inicializálása (kinullázása)
 * @details Az visszaadot struktúra minden eleme 0 értéket kap. Mivel nincs dinamikusan foglalt változója, ezért nem kell
 * felszabadítani.
 * @return Egy EventloopVariables struktúra
 */
EventloopVariables EventloopVariables_Init(void);
/**
 * @brief Frissíti a deltaT elemének értékét.
 * @details Két hívás között eltelt időt számítja ki az SDL_GetPerformanceCounter() és az SDL_GetPerformanceFrequency()
 * segítségével. Az animációk időzítésére szolgál. 
 * @param ev_vars A változtatandó EventloopVariables struktúra pointere.
 */
void Delta_Update(EventloopVariables * ev_vars);
/**
 * @brief Megvizsgálja, hogy a kurzor a megadott SDL_Rect-en belül van-e.
 * @details Egy elemre történő kattintás vizsgálatára alkalmas. Az SDL_MOUSEBUTTONDOWN és az SDL_MOUSEBUTTONUP közötti
 * változást kiszűri, azaz csak akkor ad vissza true-t, ha a kurzor az SDL_Rect-en belül volt mindkét eseménykor.
 * @param ev_vars Egy EventloopVariables struktúra.
 * @param bound A megvizsgálandó SDL_Rect.
 * @return true, ha a kurzor az SDL_Rect-en belül volt SDL_MOUSEBUTTONDOWN és SDL_MOUSEBUTTONUP-kor, egyébként false
 */
bool CursorInRect(EventloopVariables ev_vars, SDL_Rect bound);

/**
 * @brief A Wheel struktúra inicializálása (kinullázása)
 * @return Egy üres Wheel struktúra
 */
Wheel Wheel_Init(void);
/**
 * @brief Wheel struktúra létrehozása
 * @details Véletlenszerűen felvesz egy szöget és lerendereli a Layer-re a csapot.
 * A hibákat a struktok létrehozó függvényei a konzolra kiírják, ennek csak a visszatérési értéke jelzi.
 * Hiba esetén nem ajánlott a struktúra használata.
 * @post Használat után a Wheel_Destroy() függvénnyel fel kell szabadítani a struktúrát.
 * @warning TILOS egy már létrehozott Wheel struktúrát átadni.
 * @param wheel Egy inicializált Wheel struktúra pointere.
 * @param window A renderert tartalmazó Window struktúra.
 * @param location A csap szülőjéhez relatív helye
 * @param wheel_texture A csap textúrája.
 * @return true, hiba esetén false
 */
bool Wheel_Create(Wheel * wheel, Window window, SDL_Rect location, SDL_Texture * wheel_texture);
/**
 * @brief Frissíti egy Wheel Layer-ét
 * @details A Wheel Layer-ét frissíti (újrarendereli) az aktuális értékei szerint.
 * Az előforduló hibákat a függvény nem írja ki a konzolra, viszont a visszatérési értéke jelzi.
 * @param wheel A frissítendő Wheel
 * @param window A renderert tartalmazó Window struktúra
 * @param pipe_texture A csöveket tartalmazó SDL_Texture*
 * @return true, hiba esetén false
 */
bool Wheel_Update(Wheel wheel, Window window, SDL_Texture * wheel_texture);
/**
 * @brief Felszabadít egy Wheel struktúrát.
 * @details A Wheel Layer-ét szabadítja fel a saját függvényével. (lásd: SDL_local.h)
 * @param wheel A felszabadítandó Wheel struktúra pointere.
 */
void Wheel_Destroy(Wheel * wheel);

/**
 * @brief Létrehozza egy AnimationQueue strázsáját.
 * @details malloc()-kal foglal egy AnimationQueue struktúrát és kinullázza. Ehhez a különböző AnimationQueue_Append függvényekkel
 * lehet hozzáfűzni elemeket.
 * @post Használat után a AnimationQueue_Destroy() függvénnyel fel kell szabadítani a struktúrát.
 * @return Egy üres AnimationQueue struktúra
 */
AnimationQueue * AnimationQueue_Init(void);
/**
 * @brief Hozzáfűz az AnimationQueue végéhez egy Wheel forgatási animációját.
 * @details A szükséges változókat beállítja és hozzáfűzi az új elemet az AnimationQueue végéhez.
 * Az előforduló hibákat a függvény nem írja ki a konzolra, viszont a visszatérési értéke jelzi.
 * @param start Az AnimationQueue strázsájának pointere
 * @param wheel A forgatandó Wheel pointere
 * @param target_angle A forgatás mértéke (fokban)
 * @param animation_speed Az animáció sebessége
 * @return true, hiba esetén false
 */
bool AnimationQueue_AppendWheelRotate(AnimationQueue * start, Wheel * wheel, double target_angle, double animation_speed);
/**
 * @brief Hozzáfűz az AnimationQueue végéhez egy Pipe forgatási animációját.
 * @details A szükséges változókat beállítja és hozzáfűzi az új elemet az AnimationQueue végéhez.
 * Az előforduló hibákat a függvény nem írja ki a konzolra, viszont a visszatérési értéke jelzi.
 * @param start Az AnimationQueue strázsájának pointere
 * @param wheel A forgatandó Pipe pointere
 * @param target_angle A forgatás mértéke (fokban)
 * @param animation_speed Az animáció sebessége
 * @return true, hiba esetén false
 */
bool AnimationQueue_AppendPipeRotate(AnimationQueue * start, Pipe * pipe, double target_angle, double animation_speed);
/**
 * @brief Hozzáfűz az AnimationQueue végéhez egy hosszú cső feltöltési animációját.
 * @details A szükséges változókat beállítja és hozzáfűzi az új elemet az AnimationQueue végéhez.
 * A hosszú cső lehet más SDL_Texture* is, de a game.c-ben csak ehhez van használva.
 * Az előforduló hibákat a függvény nem írja ki a konzolra, viszont a visszatérési értéke jelzi.
 * @param start Az AnimationQueue strázsájának pointere
 * @param render_location A renderelendő hosszú cső helye
 * @param filled_texture A hosszú cső textúrája
 * @param animation_speed Az animáció sebessége
 * @param direction Az animáció iránya
 * @return true, hiba esetén false
 */
bool AnimationQueue_AppendLongFill(AnimationQueue * start, SDL_Rect render_location, SDL_Texture * filled_texture, double animation_speed, AnimationDirection direction);
/**
 * @brief Hozzáfűz az AnimationQueue végéhez egy Pipe feltöltési animációját.
 * @details A szükséges változókat beállítja és hozzáfűzi az új elemet az AnimationQueue végéhez.
 * Hiba esetén megpróbálja a töltést kihagyni.
 * Az előforduló hibákat a függvény nem írja ki a konzolra, viszont a visszatérési értéke jelzi.
 * @param start Az AnimationQueue strázsájának pointere
 * @param renderer Az ablak rendererje (csak hiba esetén használt)
 * @param pipe A feltöltendő Pipe pointere
 * @param filled_texture A Pipe feltöltött textúrája
 * @param animation_speed Az animáció sebessége
 * @param direction Az animáció iránya
 * @return true, hiba esetén false
 */
bool AnimationQueue_AppendPipeFill(AnimationQueue * start, SDL_Renderer * renderer, Pipe * pipe, SDL_Texture * filled_texture, double animation_speed, AnimationDirection direction);
/**
 * @brief A megadott elemet kitörli az AniamtionQueue-ból és visszaadja az előtte lévő pointerét.
 * @warning TILOS a strázsát átadni.
 * @param start Az AnimationQueue strázsájának pointere
 * @param element A törlendő elem pointere
 * @return A törlendő elem előtti elem pointere
 */
AnimationQueue * AnimationQueue_RemoveElement(AnimationQueue * start, AnimationQueue * element);
/**
 * @brief Felszabadít egy AnimationQueue láncolt listát.
 * @details A strázsáját is felszabadítja. Használat után KELL használni.
 * @param start Az AnimationQueue strázsájának pointere
 */
void AnimationQueue_Destroy(AnimationQueue * start);

/**
 * @brief A játékmenü függvénye.
 * @details Az SDL és a GameStruct inicializálása/létrehozása után meg lehet hívni.
 * Az előforduló hibákat a függvény kiírja a konzolra.
 * Súlyosabb hiba esetén a függvény false értékkel leáll.
 * @param game_struct A létrehozott GameStruct.
 * @return true, súlyos hiba esetén false 
 */
bool Menu(GameStruct game_struct);
/**
 * @brief A játék függvénye.
 * @details Az előforduló hibákat a függvény kiírja a konzolra.
 * Súlyosabb hiba esetén a függvény false értékkel leáll.
 * @note A Menu() függvény már képes meghívni, magában ne hívjuk meg, mert nem garantált a működése.
 * @param game_struct A létrehozott GameStruct.
 * @param betoltott Ha betöltés volt a Menu()-ben választva, akkor a betöltött PipeGrid.
 * @param entry_y Ha betöltés volt a Menu()-ben választva, akkor a betöltött PipeGrid bejáratánk sora.
 * @param exit_y Ha betöltés volt a Menu()-ben választva, akkor a betöltött PipeGrid kijáratának sora.
 * @return true, ha a Menu()-be lépünk vissza, false, ha új játék kezdődik, kilépés történt, illetve néhány súlyosabb hiba esetén
 */
bool Game(GameStruct game_struct, PipeGrid * betoltott, int entry_y, int exit_y);
/**
 * @brief A nyert kép függvénye.
 * @details Az előforduló hibákat a függvény kiírja a konzolra.
 * Súlyosabb hiba esetén a függvény false értékkel leáll.
 * @note A Game() függvény már képes meghívni, magában ne hívjuk meg, mert nem garantált a működése.
 * @param game_struct A létrehozott GameStruct.
 * @param state A Game() függvény aktuális renderelt képe.
 * @param copy A PipeGrid pálya betöltéskori másolata.
 * @param entry_y A PipeGrid bejáratának sora.
 * @param exit_y A PipeGrid kijáratának sora.
 * @return true, ha a Menu()-be lépünk vissza, false, ha új játék kezdődik, kilépés történt, illetve néhány súlyosabb hiba esetén
 */
bool Win(GameStruct game_struct, Layer state, PipeGrid copy, int entry_y, int exit_y);

#endif
/* game.h vége */