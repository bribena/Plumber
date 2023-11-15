

#ifndef PIPE_H
#define PIPE_H

#include "SDL_local.h"

#define DEFAULT_PIPE_SIZE 50


typedef enum ElemiResz {
    Levego = 'X',
    Cso = 'O',
    Ismeretlen = '?',
    Rossz = '.',
    Jo = '*'
} ElemiResz;

typedef enum PipeType {
    PipeType_None, PipeType_Straight, PipeType_Curved, PipeType_Tshaped, PipeType_Crossing
} PipeType;

typedef ElemiResz Elem[3][3];

typedef struct Pipe {
    Elem tenyleges;
    SDL_Rect texture_loc;
    Layer rendered;
    PipeType type;
    double szog;
} Pipe;
typedef struct PipeGrid {
    Pipe **matrix;
    int x, y;
} PipeGrid;

void Elem_Init(Elem elem);
void Elem_Copy(Elem hova, Elem honnan);
void Elem_RotatePos(Elem elem);
void Elem_RotateNeg(Elem elem);

Pipe Pipe_Init(void);
void Pipe_CreateFromElem(Pipe * pipe, Window window, SDL_Texture * pipe_texture, SDL_Rect base_location);
void Pipe_Rotate(Pipe * pipe, int irany);
void Pipe_Render(Pipe pipe, Window window);
void Pipe_Destroy(Pipe * pipe);

PipeGrid PipeGrid_Init(void);
void PipeGrid_Create(PipeGrid * pipegrid, int x, int y);
void PipeGrid_GenerateGrid(PipeGrid * pipegrid, SDL_Point entry, SDL_Point exit);
void PipeGrid_Shuffle(PipeGrid pipegrid);
void PipeGrid_Feldolgoz(PipeGrid pipegrid, Window window, SDL_Texture * pipe_texture, SDL_Rect base_location);
void PipeGrid_Render(PipeGrid pipegrid, Window window);
void PipeGrid_Destroy(PipeGrid * pipegrid);

#endif