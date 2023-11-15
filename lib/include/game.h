

#ifndef GAME_H
#define GAME_H

#include "SDL_local.h"
#include "pipe.h"
#include "sfd.h"

#define DEFAULT_WINDOW_WIDTH 1600
#define DEFAULT_WINDOW_HEIGHT 900

#define MIN_X_SIZE 18
#define MIN_Y_SIZE 14

#define WHEEL_SIZE_MULTIPLIER 1.75

typedef struct GameStruct {
    Window window;
    Assets assets;
    Fonts fonts;
} GameStruct;

typedef struct EventloopVariables {
    SDL_Event event;
    bool running;
    bool clicked;
    SDL_Point prevCursor;
    Uint64 currTime;
    Uint64 prevTime;
    double deltaT;
} EventloopVariables;

typedef struct Wheel {
    Layer layer;
    double szog;
} Wheel;

GameStruct GameStruct_Init(void);
bool GameStruct_Create(GameStruct * game_struct);
void GameStruct_Destroy(GameStruct * game_struct);

EventloopVariables EventloopVariables_Init(void);
void Delta_Update(EventloopVariables * ev_vars);
bool CursorInRect(EventloopVariables ev_vars, SDL_Rect bound);

Wheel Wheel_Init(void);
void Wheel_Create(Wheel * wheel, Window window, SDL_Rect location, SDL_Texture * wheel_texture);
void Wheel_Update(Wheel wheel, Window window, SDL_Texture * wheel_texture);
void Wheel_Destroy(Wheel * wheel);

bool Menu(GameStruct game_struct);
bool Game(GameStruct game_struct, PipeGrid * betoltott, int entry_y, int exit_y);

#endif