x86_64-w64-mingw32-gcc -Ilib/include -Llib/lib -Ilib/osdialog -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -o plumber.exe main.c lib/src/* lib/osdialog/osdialog.c lib/osdialog/osdialog_win.c SDL2.dll SDL2_image.dll SDL2_mixer.dll SDL2_ttf.dll -municode -lcomdlg32