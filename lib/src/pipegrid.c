/**
 * @file pipegrid.c
 * @brief A pipe.h-ban deklarált PipeGrid struktúrához kapcsolódó függvények kódjait tartalmazza a fájl.
 */

#include "pipe.h"
#include "debugmalloc.h"


PipeGrid PipeGrid_Init(void) {
    return (PipeGrid){NULL, 0, 0};
}

bool PipeGrid_Create(PipeGrid * pipegrid, int x, int y) {
    pipegrid->matrix = (Pipe**)malloc(sizeof(Pipe*) * y);
    if (pipegrid->matrix == NULL) return false;

    Pipe * tmp = (Pipe*)malloc(sizeof(Pipe) * x * y);
    if (tmp == NULL) {
        free(pipegrid->matrix);
        pipegrid->matrix = NULL;
        return false;
    }

    for (int i = 0; i < y; i++){
        pipegrid->matrix[i] = tmp + i * x;
        for (int j = 0; j < x; j++)
            pipegrid->matrix[i][j] = Pipe_Init();
    }

    pipegrid->x = x;
    pipegrid->y = y;

    return true;     
}

void PipeGrid_Destroy(PipeGrid * pipegrid) {
    for (int i = 0; i < pipegrid->y; i++)
        for (int j = 0; j < pipegrid->x; j++)
            Pipe_Destroy(&pipegrid->matrix[i][j]);
    
    free(pipegrid->matrix[0]);
    free(pipegrid->matrix);
}

PipeGrid PipeGrid_CreateCopy(PipeGrid source) {
    PipeGrid new = PipeGrid_Init();
    if (!PipeGrid_Create(&new, source.x, source.y))
        return new;
    
    for (int i = 0; i < new.y; i++) {
        for (int j = 0; j < new.x; j++)
        {
            Elem_Init(new.matrix[i][j].tenyleges);
            Elem_Copy(new.matrix[i][j].tenyleges, source.matrix[i][j].tenyleges);
        }
    }
        
    return new;
}


static void LabyrinthGenerator(PipeGrid * pipegrid, SDL_Point current) {
    typedef enum Irany {fel, le, jobbra, balra} Irany;

    Irany iranyok[4] = {fel, le, jobbra, balra};

    int c = 0;
    pipegrid->matrix[current.y][current.x].tenyleges[1][1] = Cso;

    // Utolsó pipe esetén
    if (current.x == pipegrid->x - 1 && pipegrid->matrix[current.y][current.x].tenyleges[1][2] == Cso) c++;

    // Irány randomizálás
    for (int i = 0; i < 4; i++) {
        int r = rand() % (4 - i);
        Irany tmp = iranyok[i];
        iranyok[i] = iranyok[r];
        iranyok[r] = tmp;
    }

    // Iránykezelés
    for (int i = 0; i < 4 && c < 2; i++) {
        switch (iranyok[i]) {
            case fel:
                if (current.y > 0 && pipegrid->matrix[current.y - 1][current.x].tenyleges[1][1] == Levego) {
                    pipegrid->matrix[current.y][current.x].tenyleges[0][1] = Cso;
                    pipegrid->matrix[current.y - 1][current.x].tenyleges[2][1] = Cso;
                    LabyrinthGenerator(pipegrid, (SDL_Point){.x=current.x, .y=current.y-1});
                    c++;
                }
                break;

            case le:
                if (current.y < pipegrid->y - 1 && pipegrid->matrix[current.y + 1][current.x].tenyleges[1][1] == Levego) {
                    pipegrid->matrix[current.y][current.x].tenyleges[2][1] = Cso;
                    pipegrid->matrix[current.y + 1][current.x].tenyleges[0][1] = Cso;
                    LabyrinthGenerator(pipegrid, (SDL_Point){.x=current.x, .y=current.y+1});
                    c++;
                }
                break;
            
            case jobbra:
                if (current.x < pipegrid->x - 1 && pipegrid->matrix[current.y][current.x + 1].tenyleges[1][1] == Levego) {
                    pipegrid->matrix[current.y][current.x].tenyleges[1][2] = Cso;
                    pipegrid->matrix[current.y][current.x + 1].tenyleges[1][0] = Cso;
                    LabyrinthGenerator(pipegrid, (SDL_Point){.x=current.x+1, .y=current.y});
                    c++;
                }
                break;
            
            case balra:
                if (current.x > 0 && pipegrid->matrix[current.y][current.x - 1].tenyleges[1][1] == Levego) {
                    pipegrid->matrix[current.y][current.x].tenyleges[1][0] = Cso;
                    pipegrid->matrix[current.y][current.x - 1].tenyleges[1][2] = Cso;
                    LabyrinthGenerator(pipegrid, (SDL_Point){.x=current.x-1, .y=current.y});
                    c++;
                }
                break;
        }

    }

    // Crossed generálás/zsákutca kezelés
    for (int i = 0; i < 4 && c == 0; i++) {
        switch (iranyok[i]) {
            case fel:
                if (current.y > 0 && pipegrid->matrix[current.y][current.x].tenyleges[0][1] == Levego) {
                    pipegrid->matrix[current.y][current.x].tenyleges[0][1] = Cso;
                    bool egyenes = pipegrid->matrix[current.y - 1][current.x].tenyleges[2][1] == Levego && pipegrid->matrix[current.y - 1][current.x].tenyleges[0][1] == Levego;
                    for (int i = 0; i < 3 && egyenes; i++)
                        egyenes = pipegrid->matrix[current.y - 1][current.x].tenyleges[1][i] == Cso;
                    if (egyenes) {
                        pipegrid->matrix[current.y - 1][current.x].tenyleges[2][1] = Cso;
                        pipegrid->matrix[current.y - 1][current.x].tenyleges[0][1] = Cso; 
                    }
                    c++;
                }
                break;
            case le:
                if (current.y < pipegrid->y - 1 && pipegrid->matrix[current.y][current.x].tenyleges[2][1] == Levego) {
                    pipegrid->matrix[current.y][current.x].tenyleges[2][1] = Cso;
                    bool egyenes = pipegrid->matrix[current.y + 1][current.x].tenyleges[2][1] == Levego && pipegrid->matrix[current.y + 1][current.x].tenyleges[0][1] == Levego;
                    for (int i = 0; i < 3 && egyenes; i++)
                        egyenes = pipegrid->matrix[current.y + 1][current.x].tenyleges[1][i] == Cso;
                    if (egyenes) {
                        pipegrid->matrix[current.y + 1][current.x].tenyleges[2][1] = Cso;
                        pipegrid->matrix[current.y + 1][current.x].tenyleges[0][1] = Cso; 
                    }
                    c++;
                }
                break;
            case jobbra:
                if (current.x < pipegrid->x - 1 && pipegrid->matrix[current.y][current.x].tenyleges[1][2] == Levego) {
                    pipegrid->matrix[current.y][current.x].tenyleges[1][2] = Cso;
                    bool egyenes = pipegrid->matrix[current.y][current.x + 1].tenyleges[1][0] == Levego && pipegrid->matrix[current.y][current.x + 1].tenyleges[1][2] == Levego;
                    for (int i = 0; i < 3 && egyenes; i++)
                        egyenes = pipegrid->matrix[current.y][current.x + 1].tenyleges[i][1] == Cso;
                    if (egyenes) {
                        pipegrid->matrix[current.y][current.x + 1].tenyleges[1][2] = Cso;
                        pipegrid->matrix[current.y][current.x + 1].tenyleges[1][0] = Cso; 
                    }
                    c++;
                }
                break;
            case balra:
                if (current.x > 0 && pipegrid->matrix[current.y][current.x].tenyleges[1][0] == Levego) {
                    pipegrid->matrix[current.y][current.x].tenyleges[1][0] = Cso;
                    bool egyenes = pipegrid->matrix[current.y][current.x - 1].tenyleges[1][0] == Levego && pipegrid->matrix[current.y][current.x - 1].tenyleges[1][2] == Levego;
                    for (int i = 0; i < 3 && egyenes; i++)
                        egyenes = pipegrid->matrix[current.y][current.x - 1].tenyleges[i][1] == Cso;
                    if (egyenes) {
                        pipegrid->matrix[current.y][current.x - 1].tenyleges[1][2] = Cso;
                        pipegrid->matrix[current.y][current.x - 1].tenyleges[1][0] = Cso; 
                    }
                    c++;
                }
                break;
        }
    }
}

void PipeGrid_GenerateGrid(PipeGrid * pipegrid, SDL_Point entry, SDL_Point exit) {
    // Generálás
    pipegrid->matrix[entry.y][entry.x].tenyleges[1][0] = Cso;
    pipegrid->matrix[exit.y][exit.x].tenyleges[1][2] = Cso;

    LabyrinthGenerator(pipegrid, entry);
}

void PipeGrid_Shuffle(PipeGrid pipegrid) {
    for (int i = 0; i < pipegrid.y; i++)
        for (int j = 0; j < pipegrid.x; j++)
            for (int k = rand() % 3; k > 0; k--)
                Pipe_Rotate(&pipegrid.matrix[i][j], 1);
}

bool PipeGrid_Feldolgoz(PipeGrid pipegrid, Window window, SDL_Texture * pipe_texture, SDL_Rect base_location) {
    for (int i = 0; i < pipegrid.y; i++) {
        for (int j = 0; j < pipegrid.x; j++) {
            SDL_Rect location = {base_location.x + j * DEFAULT_PIPE_SIZE, base_location.y + i * DEFAULT_PIPE_SIZE, DEFAULT_PIPE_SIZE, DEFAULT_PIPE_SIZE};
            if (!Pipe_CreateFromElem(&pipegrid.matrix[i][j], window, pipe_texture, location)) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Feldolgozási hiba: %s", SDL_GetError());
                return false;
            }
        }
    }
    return true;
}

bool PipeGrid_Render(PipeGrid pipegrid, Window window) {
    for (int i = 0; i < pipegrid.y; i++) {
        for (int j = 0; j < pipegrid.x; j++) {
            if (!Pipe_Render(pipegrid.matrix[i][j], window)) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderelési hiba: %s", SDL_GetError());
                return false;
            }
        }
    }
    return true;
}

