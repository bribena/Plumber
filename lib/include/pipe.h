

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

typedef struct SolvedPipeGrid {
    Pipe * pipe;
    int x, y;
    struct SolvedPipeGrid * next;
} SolvedPipeGrid;

void Elem_Init(Elem elem);
void Elem_Copy(Elem hova, Elem honnan);
void Elem_RotatePos(Elem elem);
void Elem_RotateNeg(Elem elem);

Pipe Pipe_Init(void);
bool Pipe_CreateFromElem(Pipe * pipe, Window window, SDL_Texture * pipe_texture, SDL_Rect base_location);
void Pipe_Rotate(Pipe * pipe, int irany);
bool Pipe_Render(Pipe pipe, Window window);
bool Pipe_UpdateLayer(Pipe pipe, Window window, SDL_Texture * pipe_texture);
void Pipe_Destroy(Pipe * pipe);

PipeGrid PipeGrid_Init(void);
bool PipeGrid_Create(PipeGrid * pipegrid, int x, int y);
PipeGrid PipeGrid_CreateCopy(PipeGrid source);
void PipeGrid_GenerateGrid(PipeGrid * pipegrid, SDL_Point entry, SDL_Point exit);
void PipeGrid_Shuffle(PipeGrid pipegrid);
bool PipeGrid_Feldolgoz(PipeGrid pipegrid, Window window, SDL_Texture * pipe_texture, SDL_Rect base_location);
bool PipeGrid_Render(PipeGrid pipegrid, Window window);
void PipeGrid_Destroy(PipeGrid * pipegrid);

SolvedPipeGrid * PipeGrid_Solve(PipeGrid pipegrid, SDL_Point entry, SDL_Point exit);
SolvedPipeGrid * SolvedPipeGrid_RemoveFirst(SolvedPipeGrid * start);
void SolvedPipeGrid_Destroy(SolvedPipeGrid * start);

#endif