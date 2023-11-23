
#ifndef SDL_LOCAL_H
#define SDL_LOCAL_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL2_gfxPrimitives.h>
#include <stdbool.h>

#define DEFAULT_FONT_SIZE 200
#define MIX_VOLUME 70


typedef struct Window {
    SDL_Window * window;
    SDL_Renderer * renderer;
    int width, height;
} Window;

typedef struct Assets {
    SDL_Texture * background;
    SDL_Texture * pipes_empty;
    SDL_Texture * pipes_full;
    SDL_Texture * wheel;
    Mix_Music * bgm;
} Assets;

typedef struct Fonts {
    TTF_Font * normal;
    TTF_Font * italic;
    TTF_Font * bold;
    TTF_Font * bold_italic;
} Fonts;

typedef struct Layer {
    SDL_Texture * layer;
    SDL_Rect location;
} Layer;


bool InitializeSDL(void);
void StartMusic(Assets assets);
void QuitSDL(void);

Window Window_Init(void);
bool Window_Create(Window * window, char * title, int width, int height);
void Window_Destroy(Window * window);

Assets Assets_Init(void);
bool Assets_Load(Assets * assets, Window window);
void Assets_Destroy(Assets * assets);

Fonts Fonts_Init(void);
bool Fonts_Load(Fonts * fonts);
void Fonts_Destroy(Fonts * fonts);

Layer Layer_Init(void);
bool Layer_Create(Layer * layer, Window window, SDL_Rect * location);
bool Layer_RenderFont(Layer * layer, Window window, TTF_Font * font, char * text, SDL_Color foreground);
void Layer_Destroy(Layer * layer);

#endif