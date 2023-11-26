# Vízvezetékszerelő/Plumber
A vízvezetékszerelő egy játék, amit a Programozás alapjai 1 tárgy nagy házijához készítettem.

GitHubon itt található meg: https://github.com/bribena/Plumber



## Telepítés

Először klónozza a repositoryt:

``````
git clone https://github.com/bribena/Plumber.git
``````

A program az SDL, az SDL_image, az SDL_mixer, az SDL_ttf, az SDL_gfxPrimitives, az osdialog és a C moduljaira épül.

Linuxon a GCC, Windowson a MinGW segítségével fordítható a program.

### SDL telepítése
A különböző SDL modulok forrásainak linkjei:
* SDL: https://github.com/libsdl-org/SDL/releases/tag/release-2.28.5
* SDL_image: https://github.com/libsdl-org/SDL_image/releases
* SDL_ttf: https://github.com/libsdl-org/SDL_ttf/releases
* SDL_mixer: https://github.com/libsdl-org/SDL_mixer/releases
* SDL_gfxPrimitives: https://www.ferzkopp.net/wordpress/2016/01/02/sdl_gfx-sdl2_gfx/

Az SDL telepítési lépései: https://wiki.libsdl.org/SDL2/Installation

#### Linux

Lehetőleg a disztribúció csomagkezelőjével szerezze be ezeket az SDL modulokat. Ha ott nem elérhető, akkor a megfelelő repository telepítési utasításait kövesse.

#### Windows

Windowshoz a megadott oldalakon találhatóak előre buildelt fájlok. Ezeket töltse le (mindegyiknél a MinGW-vel kompatibilis zipfájlt). A zipfájlokat az alábbi módon csomagolja ki:

- A dll fájlok (bin mappa tartalma, kivétel SDL_gfxPrimitives) kerüljenek a projekt gyökérmappájába
- A header fájlok (include mappa tartalma, kivétel SDL_gfxPrimitives) kerüljenek a projekt lib/include mappájába
- A lib mappa pedig mappástul a projekt lib mappájába

Az SDL_gfxPrimitives esetén a megfelelő fájlok (.dll, .h, .a/.o) megtalálhatóak a zipjében. Ezeket a fentiek szerint helyezze el a projekt megfelelő mappáiban.

### osdialog telepítése

GitHub: https://github.com/AndrewBelt/osdialog

Mindkét rendszeren a repositoryt klónozza a projekt lib mappájába.

Linuxon az osdialog a GTK 2.0-t használja, ezt is szerezze be a disztribúciójának a csomagkezelőjével.

Megjegyzés: az osdialog a debugmalloc miatt hibát dobhat. Erre két javítás van:

1. Az osdialog.h-ban az include-ok alá írja be, hogy `#include "debugmalloc.h"`
2. A projekt fájlaiból ki kell venni az `#include "debugmalloc.h"` sorokat.



## Fordítás

### Linux

A `build.sh`  futtatásával létrejön a `plumber` bináris fájl, amennyiben az SDL modulok rendszerszinten elérhetőek. Egyéb esetben manuálisan kell fordítani a programot a megfelelő helyek megadásával.

 A `build.sh` gcc-vel fordítja le a kódot.

### Windows

A `build.bat` futtatásával létrejön a `plumber.exe` bináris fájl, amennyiben megfelelően vannak elhelyezve a külső modulok fájlai. Egyéb esetben manuálisan kell fordítani a programot a megfelelő helyek megadásával.

A `build.bat` x86_64-w64-mingw32-gcc-vel fordítja a kódot.



## A játék menete

A játék induláskor a főmenübe dobja a játékost. Innen 3 opció tárul elé:

- Játék: Létrehoz egy véletlenszerű pályát, amit ki lehet játszani.
- Pálya betöltése: Egy, a program által generált .plm fájlt tölt be. Néhány mintafájl található a projekt testfiles mappájában. Ezután elindítja a játékot.
- Kilépés

A pályán belül 1-1 csőre kattintva el lehet forgatni azt (bal kattintással az óra haladási irányával megegegyezően, jobb kattintással pedig ellentétesen). A játék célja, hogy a bemenettől (hosszú cső a bal oldalon) a kimenetig (hosszú cső a jobb oldalon) a pályán levő csövekkel a játékos elvezesse a vizet. A vizet elindítani csak helyes megoldás esetén lehet a bemeneten található csappal, bal kattintással. Helyes megoldásnak egy a bemenettől a kimenetig vezető csőpálya számít. Az egyes csövekből nincsen "kifolyás", azaz ha már egy irányba tudja tovább tölteni a vizet, akkor a megoldás szempontjából megfelelő.

Egy különleges elem van, az "egymáson átmenő", ami két különböző irányba menő egyenesnek felel meg.

A játék közben bármikor kimenthető a pálya *aktuális* állapota.

Győzelemkor a pálya *kezdésekor előforduló* állapota menthető ki. Azaz, ha a Pálya betöltésével lett elkezdve a játék, akkor a betöltött állapotot lehet kimenteni, játék esetén pedig a legenerált pályát.

A kimentett fájl nevében ne legyen szóköz, ékezet és speciális karakter.

Futás után visszaadott kódok:
	0: Nem történt hiba
	1: Nem sikerült az SDL inicializálása
	2: Nem sikerült az ablakot létrehozni/a fájlokat betölteni
	3: A játék futása során hiba történt

### A generált fájlok struktúrája (.plm)

A fájl első sora 4 adatot tartalmaz, ebben a sorrendben:

- Az egy sorban található csövek számát
- Az egy oszlopban található csövek számát
- A pálya bemenetének sora
- A pálya kimenetének sora

Ezután 3x3-as blokkokban tartalmazza a pályán szereplő csöveket szöveges formátumban.

Bármilyen fájl, ami ennek a struktúrának nem felel meg, hibás fájlnak számít.



## Hibák

A programmal kapcsolatos hibák a programozói dokumentációban megtalálhatóak (a projekt docs mappájában). Ezeken kívül a hibák nagy részét kiírja a konzolra a program, ha előfordulnak. Ekkor vagy összeomlik a program, vagy az adott funkció nélküll fut tovább.



Készítette: Bihari Bence