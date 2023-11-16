#include "pipe.h"
#include "debugmalloc.h"

Pipe Pipe_Init(void) {
    Pipe pipe;
    Elem_Init(pipe.tenyleges);
    pipe.texture_loc = (SDL_Rect){0,0,0,0};
    pipe.rendered = Layer_Init();
    pipe.type = PipeType_None;
    pipe.szog = 0;
    return pipe;
}

void Pipe_CreateFromElem(Pipe * pipe, Window window, SDL_Texture * pipe_texture, SDL_Rect location) {
    int c = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (pipe->tenyleges[i][j] == Cso) c++;

    switch (c) {
        // Egyenes/curved (elfelejtettem, mi magyarul)
        case 3:
            // Vsz egyenes
            if (pipe->tenyleges[1][0] == Cso && pipe->tenyleges[1][2] == Cso) {
                pipe->texture_loc = (SDL_Rect){0, 0, 500, 500};
                pipe->szog = ((rand() % 2) * 180);
                pipe->type = PipeType_Straight;
            }
            // Fg egyenes
            else if (pipe->tenyleges[0][1] == Cso && pipe->tenyleges[2][1] == Cso) {
                pipe->texture_loc = (SDL_Rect){0, 0, 500, 500};
                pipe->szog = ((rand() % 2) * 180) + 90;
                pipe->type = PipeType_Straight;
            }
            // curved
            else {
                pipe->texture_loc = (SDL_Rect){500, 0, 500, 500};
                if (pipe->tenyleges[1][0] == Cso && pipe->tenyleges[2][1] == Cso) 
                    pipe->szog = 0;
                else if (pipe->tenyleges[1][0] == Cso && pipe->tenyleges[0][1] == Cso)
                    pipe->szog = 90;
                else if (pipe->tenyleges[1][2] == Cso && pipe->tenyleges[0][1] == Cso) 
                    pipe->szog = 180;
                else 
                    pipe->szog = 270;
                pipe->type = PipeType_Curved;
            }
            break;

        // T
        case 4:
            pipe->texture_loc = (SDL_Rect){0, 500, 500, 500};
            if (pipe->tenyleges[1][0] == Levego && pipe->tenyleges[1][2] == Cso) pipe->szog = 90;
            else if (pipe->tenyleges[0][1] == Levego && pipe->tenyleges[2][1] == Cso) pipe->szog = 180;
            else if (pipe->tenyleges[1][0] == Cso && pipe->tenyleges[1][2] == Levego) pipe->szog = 270;
            // else pipe->szog = 0;
            pipe->type = PipeType_Tshaped;
            break;

        // Keresztezett
        case 5:
            pipe->texture_loc = (SDL_Rect){500, 500, 500, 500};
            pipe->szog = ((rand() % 4) * 90);
            pipe->type = PipeType_Crossing;
            break;
    }

    Layer_Create(&pipe->rendered, window, &location);
    SDL_SetRenderTarget(window.renderer, pipe->rendered.layer);
    SDL_RenderCopyEx(window.renderer, pipe_texture, &pipe->texture_loc, NULL, pipe->szog, NULL, SDL_FLIP_NONE);
}

void Pipe_Rotate(Pipe * pipe, int irany) {
    if (pipe->type == PipeType_Crossing) return;

    if (irany == 1) Elem_RotatePos(pipe->tenyleges);
    else if (irany == -1) Elem_RotateNeg(pipe->tenyleges);
}

void Pipe_Destroy(Pipe * pipe) {
    Layer_Destroy(&pipe->rendered);
}


void Pipe_Render(Pipe pipe, Window window) {
    SDL_RenderCopy(window.renderer, pipe.rendered.layer, NULL, &pipe.rendered.location);
}

void Pipe_UpdateLayer(Pipe pipe, Window window, SDL_Texture * pipe_texture) {
    SDL_SetRenderTarget(window.renderer, pipe.rendered.layer);
    SDL_SetRenderDrawColor(window.renderer, 0, 0, 0, 0);
    SDL_RenderClear(window.renderer);
    SDL_RenderCopyEx(window.renderer, pipe_texture, &pipe.texture_loc, NULL, pipe.szog, NULL, SDL_FLIP_NONE);
    SDL_SetRenderTarget(window.renderer, NULL);
}