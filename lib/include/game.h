

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

#define WHEEL_ANIMATION_SPEED 2
#define PIPE_ANIMATION_SPEED 3
#define LONG_PIPE_ANIMATION_SPEED 10
#define PIPE_FILL_ANIMATION_SPEED 20


typedef enum AnimationType {
    AnimationType_None, AnimationType_Rotate, AnimationType_Fill
} AnimationType;

typedef enum AnimationDirection {
    AnimationDirection_ToRight, AnimationDirection_ToLeft, AnimationDirection_ToUp, AnimationDirection_ToDown
} AnimationDirection;

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

// Strázsás láncolt lista
typedef struct AnimationQueue {
    bool active;
    AnimationType type;
    bool is_pipe;
    double animation_speed;

    Pipe * pipe;
    Wheel * wheel;
    
    double target_angle;
    double rendered_angle;
    double * angle;

    AnimationDirection direction;
    double target_length;
    double target_coor;
    double ratio;
    SDL_Rect texture_location;
    SDL_Rect render_location;
    SDL_Texture * filled_texture;

    struct AnimationQueue * next;
} AnimationQueue;

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

AnimationQueue * AnimationQueue_Init(void);
void AnimationQueue_AppendWheelRotate(AnimationQueue * start, Wheel * wheel, double target_angle, double animation_speed);
void AnimationQueue_AppendPipeRotate(AnimationQueue * start, Pipe * pipe, double target_angle, double animation_speed);
void AnimationQueue_AppendLongFill(AnimationQueue * start, SDL_Rect render_location, SDL_Texture * filled_texture, double animation_speed, AnimationDirection direction);
void AnimationQueue_AppendPipeFill(AnimationQueue * start, SDL_Renderer * renderer, Pipe * pipe, SDL_Texture * filled_texture, double animation_speed, AnimationDirection direction);
AnimationQueue * AnimationQueue_RemoveElement(AnimationQueue * start, AnimationQueue * element);
void AnimationQueue_Destroy(AnimationQueue * start);

bool Menu(GameStruct game_struct);
bool Game(GameStruct game_struct, PipeGrid * betoltott, int entry_y, int exit_y);
bool Win(GameStruct game_struct, Layer state, PipeGrid copy, int entry_y, int exit_y);

#endif