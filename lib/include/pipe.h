/**
 * @file pipe.h
 * 
 * @brief A játékban használt csövekhez kapcsolódó függvényeket és adatszerkezeteket tartalmazza a fájl
 * 
 * @details
 * @pre Mivel az SDL_local.h-t használja, az SDL inicializálása szükséges a modul megfelelő működéséhez.
 * 
 * Struktúrák, enumok:
 * -------------------
 * 3 fő struktúrát tartalmaz a fájl:
 *  - Pipe: Egy cső adatait tartalmazza.
 *  - PipeGrid: Csövek mátrixát és a mátrix szélességét és magasságát tartalmazza.
 *  - SolvedPipeGrid: Egy PipeGrid megoldását tartalmazza. A megfelelő csöveket és indexeiket
 *    strázsa nélküli láncolt listában tartalmazza.
 * 
 * A struktúrák használata többnyire az alábbi módon történik:
 * @code{.c}
 * Struktúra név = Struktúra_Init(); // Az init egy üres elemet ad vissza, nem foglal magának memóriát
 * Struktúra_LétrehozóFüggvény(&struktúra, paraméterek); // Lásd lentebb
 * 
 * // Használat
 *
 * Struktúra_Destroy(&struktúra); // A struktúrákat, amik tartalmaznak dinamikusan foglalt elemeket, fel kell szabadítani
 * @endcode
 * Ez alól van kivétel, lásd lentebb.
 * 
 * Maga a cső az Elem típusú 3x3-as mátrixban van leképezve, ami ElemiResz enumokból épül fel. Ebben van nyilvántartva maga a cső
 * és a megoldás során az "áramlás" útvonalanák helyessége.
 * 
 * Ezen kívül a PipeType enum egy Pipe típusát határozza meg.
 * 
 * Függvények:
 * -----------
 * A modul a struktúrákhoz kapcsolódó függvényeket tartalmazza.
 * 
 * Példakód:
 * ---------
 * @code{.c}
 * #include "SDL_local.h" // Explicit include
 * #include "pipe.h"
 * 
 * int main(int argc, char * argv[]) {
 *     (void)argc, (void)argv; // Warning elkerülése
 *     InitializeSDL(); // SDL inicializálása
 * 
 *     PipeGrid pipegrid = PipeGrid_Init(); // PipeGrid inicializálása
 *     PipeGrid_Create(&pipegrid, 10, 10); // 10x10-es PipeGrid létrehozása
 *     PipeGrid_GenerateGrid(&pipegrid, (SDL_Point){0, 0}, (SDL_Point){9, 9}); // PipeGrid feltöltése
 *     
 *     SolvedPipeGrid * solved = PipeGrid_Solve(pipegrid, (SDL_Point){0, 0}, (SDL_Point){9, 9}); // PipeGrid megoldása
 * 
 *     // Megoldás koordinátáinak kiírása
 *     for (SolvedPipeGrid * i = solved; i != NULL; i = i->next)
 *         printf("%d, %d\n", i->x, i->y);
 * 
 *     SolvedPipeGrid_Destroy(solved); // SolvedPipeGrid felszabadítása
 *     PipeGrid_Destroy(&pipegrid); // PipeGrid felszabadítása
 *     QuitSDL(); // SDL leállítása
 *     return 0;
 * }
 * @endcode
 * 
 * A fenti példakód létrehoz egy 10x10-es PipeGrid-et, majd generál ebből egy pályát.
 * Ezután megoldja a pályát, majd kiírja stdout-on a megfelelő csövek koordinátáit (indexeit).
 * 
 * Megjegyzések:
 * -------------
 *  - Minden innen származó struktúrának a "legtetejét" kell felszabadítani. Ezalatt azt kell érteni, hogy például a PipeGrid
 *    tartalmazza a szintén dinamikusan foglalt Pipe-ot, de a PipeGrid_Destroy() a tárolt Pipe-ot is felszabadítja. Erről bővebben
 *    az adott függvény dokumentációja ír.
 * 
 * Kapcsolódó kódfájlok:
 * ---------------------
 *  - elem.c
 *  - pipe.c
 *  - pipegrid.c
 *  - solve.c
*/

/* pipe.h eleje */
#ifndef PIPE_H
#define PIPE_H

#include "SDL_local.h"

/**
 * @brief A csövek alapértelmezett magassága és szélessége (pixelben)
*/
#define DEFAULT_PIPE_SIZE 50

/**
 * @enum ElemiResz
 * @brief Egy-egy Elem részeinek lehetséges állapotai
 * @details Egy Elem generálás után Levego és Cso ElemiResz-ekből áll. A másik három állapot a PipeGrid_Solve() függvénynél
 * jön elő.
 */
typedef enum ElemiResz {
    /**
     * @details Egy Elem-en belül a levegőt jelöli (azaz nem Cso).
    */
    Levego = 'X',
    /**
     * @details Egy Elem-en belül a csövet jelöli. Ebben mehet a víz. 
    */
    Cso = 'O',
    /**
     * @details A PipeGrid_Solve() során egy eldöntetlen útvonalnál használt.
     */
    Ismeretlen = '?',
    /**
     * @details A PipeGrid_Solve() során egy rossz útvonalnál használt.
     */
    Rossz = '.',
    /**
     * @details A PipeGrid_Solve() során egy jó útvonalnál használt.
     */
    Jo = '*'
} ElemiResz;

/**
 * @enum PipeType
 * @brief A Pipe típusának enumja
 * @details Nincs túl sok helyen használva, viszont hasznos információt mond el egy Pipe-ról. Emellett ezek a lehetséges típusok.
 */
typedef enum PipeType {
    /**
     * @details Az feldolgozás előtti alapértelmezett érték. Egyébként nem lehetséges.
     */
    PipeType_None,
    /**
     * @details Egyenes cső.
     */
    PipeType_Straight,
    /**
     * @details Kanyarodó cső.
    */
    PipeType_Curved,
    /**
     * @details T alakú cső.
    */
    PipeType_Tshaped,
    /**
     * @details Kereszteződő/Egymáson átmenő cső.
     */
    PipeType_Crossing
} PipeType;

/**
 * @typedef Elem
 * @brief Egy Elem-et reprezentáló 3x3-as mátrix
 * @details Egy-egy Pipe egyszerűbb alakja. A PipeGrid_GenerateGrid() és a PipeGrid_Solve() függvények egyszerűbb működéséhez
 * szükséges.
*/
typedef ElemiResz Elem[3][3];

/**
 * @struct Pipe
 * @brief Egy cső adatainak tárolására szolgáló struktúra
 * @details Egy cső feldolgozásához, illetve egy cső rendereléséhez szükséges összes adatot tárolja.
 */
typedef struct Pipe {
    /**
     * @details A cső PipeGrid_GenerateGrid()-ben és PipeGrid_Solve()-ban használt alakja.
     */
    Elem tenyleges;
    /**
     * @details A cső textúrájának helye az assets/img/sprites_empty.png-n belül (ez az Assets struktúra pipes_empty eleme). 
     */
    SDL_Rect texture_loc;
    /**
     * @details A cső rétege. Azért kell, hogy csakis egyetlen csőt meg lehessen változtatni.
     */
    Layer rendered;
    /**
     * @details A cső típusa.
     */
    PipeType type;
    /**
     * @details A cső szöge. Rendereléskor (animáláskor) használt.
     */
    double szog;
} Pipe;

/**
 * @struct PipeGrid
 * @brief A Pipe-ok mátrixát tároló struktúra
 * @details Egy y*x-es mátrix, amiben Pipe-ok vannak. Indexeléskor oda kell figyelni, hogy először a sor (y), majd az oszlop (x)
 * indexét kell megadni.
 */
typedef struct PipeGrid {
    /**
     * @details A Pipe mátrix.
     */
    Pipe **matrix;
    /**
     * @var x
     * @details Sorszélesség ("x tengely")
     */
    /**
     * @var y
     * @details Oszlopmagasság ("y tengely")
     */
    int x, y;
} PipeGrid;

/**
 * @struct SolvedPipeGrid
 * @brief Egy PipeGrid megoldását tároló láncolt lista
 * @details Egy PipeGrid megoldását tárolja. A megfelelő Pipe-okat és indexeiket strázsa nélküli láncolt listában tárolja.
 * Nincsen Init() függvénye, helyette egy PipeGrid-ből hozzza létre magát.
 */
typedef struct SolvedPipeGrid {
    /**
     * @details Egy Pipe pointere (a PipeGrid-ben).
     */
    Pipe * pipe;
    /**
     * @var x
     * @details A Pipe oszlop (x) indexe a PipeGrid-ben.
     */
    /**
     * @var y
     * @details A Pipe sor (y) indexe a PipeGrid-ben.
     */
    int x, y;
    /**
     * @details A láncolt lista következő elemére mutató pointer.
     */
    struct SolvedPipeGrid * next;
} SolvedPipeGrid;

/**
 * @brief Egy üres (csupa Levego-s) Elem-et álltít elő.
 * @details Az átadott Elem-et feltölti Levego-vel. A C nyelv sajátossága miatt tér el a többi Init függvénytől.
 * @note Ne használd. Csak azért van a header-ben, mert máshol van definiálva, mint ahol használva.
 * @param elem Az inicializálandó Elem
 */
void Elem_Init(Elem elem);
/**
 * @brief Átmásol egy Elem tartalmát egy másikba.
 * @details A honnan Elem tartalmát másolja át a hova Elem-be. A hova Elem tartalmát felülírja. A honnan elem tartalmát nem 
 * változtatja, viszont a fordító warningot ad, ha const a paraméter.
 * @note Ne használd. Csak azért van a header-ben, mert máshol van definiálva, mint ahol használva.
 * @param hova A cél Elem
 * @param honnan A forrás Elem
 */
void Elem_Copy(Elem hova, Elem honnan);
/**
 * @brief Elforgat egy Elem-et az óramutató járásával megegyező irányba.
 * @note Ne használd. Csak azért van a header-ben, mert máshol van definiálva, mint ahol használva.
 * @param elem A változtatandó Elem
 */
void Elem_RotatePos(Elem elem);
/**
 * @brief Elforgat egy Elem-et az óramutató járásával ellentétes irányba.
 * @note Ne használd. Csak azért van a header-ben, mert máshol van definiálva, mint ahol használva.
 * @param elem A változtatandó Elem
 */
void Elem_RotateNeg(Elem elem);

/**
 * @brief Pipe struktúra inicializálása (kinullázása/alapértelmezett értékek beállítása)
 * @return Egy üres Pipe struktúra
 */
Pipe Pipe_Init(void);
/**
 * @brief Pipe struktúra létrehozása a benne található Elem alapján.
 * @details  A függvény létrehozza a Pipe Layer-ét, illetve beállítja a szülőjéhez relatív helyét (lásd: Layer),
 * felvesz egy szöget és beállítja a PipeType-ot, illetve a textúrájának helyét. Az előforduló hibákat a függvény nem 
 * írja ki a konzolra, viszont a visszatérési értéke jelzi.
 * Hiba esetén nem ajánlott a létrejött struktúrát használni.
 * @post Mivel létrehoz egy Layer-t, használat után a Pipe-ot fel kell szabadítani a Pipe_Destroy() függvénnyel.
 * @note A projekten belül található Create függvényektől eltérően inkább feldolgoz, mintsem létrehoz. Emiatt is lehetőleg
 * csak a PipeGrid_Feldolgoz() függvényen belül maradjon használva.
 * @param pipe A feldolgozandó Pipe pointere
 * @param window A renderert tartalmazó Window struktúra
 * @param pipe_texture A csöveket tartalmazó SDL_Texture*
 * @param base_location A Pipe relatív helye
 * @return true, hiba esetén false
 */
bool Pipe_CreateFromElem(Pipe * pipe, Window window, SDL_Texture * pipe_texture, SDL_Rect base_location);
/**
 * @brief Elforgatja egy Pipe Elem-ét
 * @details Az iránynak megfelelően elforgatja a Pipe Elem-ét. Ha az irány 1, az óramutató járásával megegyező irányba, ha -1,
 * akkor az óramutató járásával ellentétes irányba. Ha a Pipe típusa PipeType_Crossing, akkor nem történik semmi.
 * @param pipe A változtatandó Pipe
 * @param irany A forgatás iránya (1: óramutató járásával megegyező, -1: óramutató járásával ellentétes)
 */
void Pipe_Rotate(Pipe * pipe, int irany);
/**
 * @brief Renderel egy Pipe-ot a jelenlegi RenderTarget-re
 * @details A Pipe Layer-ét rendereli a jelenlegi RenderTarget-re. Fontos, hogy a RenderTarget a "szülője" legyen, azaz
 * az, amihez a Pipe Layer-ének helye relatív. Egyéb esetben nem garantált a helyes működés.
 * Az előforduló hibákat a függvény nem írja ki a konzolra, viszont a visszatérési értéke jelzi.
 * @param pipe A renderelendő Pipe
 * @param window A renderert tartalmazó Window struktúra
 * @return true, hiba esetén false
 */
bool Pipe_Render(Pipe pipe, Window window);
/**
 * @brief Frissíti egy Pipe Layer-ét
 * @details A Pipe Layer-ét frissíti (újrarendereli) az aktuális értékei szerint.
 * Az előforduló hibákat a függvény nem írja ki a konzolra, viszont a visszatérési értéke jelzi.
 * @param pipe A frissítendő Pipe
 * @param window A renderert tartalmazó Window struktúra
 * @param pipe_texture A csöveket tartalmazó SDL_Texture*
 * @return true, hiba esetén false
 */
bool Pipe_UpdateLayer(Pipe pipe, Window window, SDL_Texture * pipe_texture);
/**
 * @brief Felszabadít egy Pipe struktúrát
 * @details Az átadott Pipe_CreateFromElem() függvénnyel létrehozott Pipe-ot felszabadítja. Lényegében a struktúra Layer-ét
 * szabadítja fel.
 * @param pipe A felaszabadítandó Pipe struktúra
 */
void Pipe_Destroy(Pipe * pipe);

/**
 * @brief PipeGrid struktúra inicializálása (kinullázása)
 * @return Egy üres PipeGrid struktúra
 */
PipeGrid PipeGrid_Init(void);
/**
 * @brief PipeGrid struktúra létrehozása
 * @details Létrehoz egy PipeGrid-et a megadott szélességgel és magassággal. A létrejövő mátrix elemeit a Pipe_Init()-tel
 * hozza létre.
 * Az előforduló hibákat a függvény nem írja ki a konzolra, viszont a visszatérési értéke jelzi.
 * Hiba esetén a struktúra mátrixa NULL.
 * @post Mivel létrehoz egy Pipe mátrixot, használat után a PipeGrid-ot fel kell szabadítani a PipeGrid_Destroy() függvénnyel.
 * @warning TILOS egy, már létrehozott, PipeGrid struktúrát átadni, mert memóriaszivárgás lehet belőle.
 * @param pipegrid A létrehozandó PipeGrid
 * @param x A PipeGrid szélessége
 * @param y A PipeGrid magassága
 * @return true, hiba esetén false
 */
bool PipeGrid_Create(PipeGrid * pipegrid, int x, int y);
/**
 * @brief Lemásol egy PipeGrid-et és visszaadja a másolatot
 * @details Csak a Pipe-ok Elem-ét másolja le, ezért ha használni akarjuk a másolatot, akkor először fel kell dolgozni a
 * PipeGrid_Feldolgoz() függvénnyel.
 * @post A másolatot a PipeGrid_Create()-tel hozza létre, így fel kell szabadítani használat után a PipeGrid_Destroy()-el.
 * @param source A forrás PipeGrid
 * @return Egy új PipeGrid, hiba esetén ennek a mátrixa NULL
 */
PipeGrid PipeGrid_CreateCopy(PipeGrid source);
/**
 * @brief Legenerál egy PipeGrid-et
 * @details A játékhoz az átadott PipeGrid-ben generál egy pályát. Ez egy rekurzív labirintusgeneráló algoritmussal történik,
 * aminek a segítségével egy véletlenszerű, megoldható pályát generál.
 * Alapul az infoc-n található algoritmus szolgált (link: https://infoc.eet.bme.hu/labirintus/), ami módosítva lett, hogy az
 * általam használt adatszerkezetekkel működjön (Pipe-on belüli Elem).
 * @warning A belépési és kilépési pontok szabadon megadhatóak, DE az entry lehetőleg az első oszlopban, az exit pedig az utolsóban
 * legyen. Egyéb esetben nem garantált a helyes működés.
 * @param pipegrid A változtatandó PipeGrid
 * @param entry A belépési pont (x: oszlopindex, y: sorindex)
 * @param exit A kilépési pont (x: oszlopindex, y: sorindex)
 */
void PipeGrid_GenerateGrid(PipeGrid * pipegrid, SDL_Point entry, SDL_Point exit);
/**
 * @brief Összekever egy PipeGrid-et
 * @details A PipeGrid-en belül lévő Pipe-ok Elem-eit véletlenszerűen elforgatja.
 * @param pipegrid Az összekeverendő PipeGrid
 */
void PipeGrid_Shuffle(PipeGrid pipegrid);
/**
 * @brief Egy PipeGrid Pipe-jainak hiányzó értékeit feltölti.
 * @details A PipeGrid-ben található Pipe-ok Elem-ei alapján feltölti a Pipe-ok hiányzó értékeit a Pipe_CreateFromElem()-el,
 * azaz létrehozza a Layer-ét, beállítja a szülőjéhez relatív helyét, felvesz egy szöget és beállítja a PipeType-ot, illetve
 * a textúrájának helyét.
 * Az előforduló hibákat a függvény kiírja a konzolra.
 * Hiba esetén nem ajánlott a struktúra használata.
 * @pre A Pipe-ok Elem-jeinek már be kell legyen állítva az értéke. Emiatt csak a "végleges" állapotban ajánlott használni, azaz
 * a PipeGrid_GenerateGrid() és a PipeGrid_Shuffle() függvények után.
 * @warning TILOS egy, már feldolgozott, PipeGrid struktúrát átadni, mert memóriaszivárgás lehet belőle.
 * @param pipegrid A feldolgozandó PipeGrid
 * @param window A renderert tartalmazó Window struktúra
 * @param pipe_texture A csöveket tartalmazó SDL_Texture*
 * @param base_location A PipeGrid megjelenítési helye
 * @return true, hiba esetén false
 */
bool PipeGrid_Feldolgoz(PipeGrid pipegrid, Window window, SDL_Texture * pipe_texture, SDL_Rect base_location);
/**
 * @brief Lerenderel egy feldolgozott PipeGrid-et az aktuális RenderTarget-re
 * @details Az előforduló hibákat a függvény kiírja a konzolra.
 * @param pipegrid Egy feldolgozott PipeGrid
 * @param window A renderert tartalmazó Window struktúra
 * @return true, hiba esetén false
 */
bool PipeGrid_Render(PipeGrid pipegrid, Window window);
/**
 * @brief Felszabadít egy PipeGrid-et
 * @details A PipeGrid-ben lévő Pipe-okat is felszabadítja.
 * @param pipegrid A felaszabadítandó PipeGrid
 */
void PipeGrid_Destroy(PipeGrid * pipegrid);

/**
 * @brief Megold egy PipeGrid-et és a megoldását egy SolvedPipeGrid-ben adja vissza.
 * @details Át kell adni a PipeGrid_GenerateGrid()-ben használt belépési és kilépési pontokat, egyébként nem fogja jól megoldani.
 * @post Mivel egy láncolt listát hoz létre a függvény, használat után a SolvedPipeGrid_Destroy()-al kell felszabadítani.
 * @param pipegrid A megoldandó PipeGrid
 * @param entry A belépési pont (x: oszlopindex, y: sorindex)
 * @param exit A kilépési pont (x: oszlopindex, y: sorindex)
 * @return A megoldás láncolt listájának első eleme, hiba vagy megoldás hiánya esetén NULL.
 */
SolvedPipeGrid * PipeGrid_Solve(PipeGrid pipegrid, SDL_Point entry, SDL_Point exit);
/**
 * @brief A SolvedPipeGrid láncolt lista első elemét felszabadítja és visszaadja a következő elem pointerét.
 * @param start A láncolt lista első elemére mutató pointer
 * @return A következő elemre mutató pointer (NULL, ha nincs több elem)
 */
SolvedPipeGrid * SolvedPipeGrid_RemoveFirst(SolvedPipeGrid * start);
/**
 * @brief Felszabadít egy SolvedPipeGrid láncolt listát
 * @param start A láncolt lista első elemére mutató pointer
 */
void SolvedPipeGrid_Destroy(SolvedPipeGrid * start);

#endif
/* pipe.h vége */