#include "pipe.h"
#include "debugmalloc.h"


typedef enum Irany {fel, le, jobbra, balra} Irany;

bool LabyrinthSolver(PipeGrid pipegrid, SDL_Point current) {
    Elem c;
    Elem_Copy(c, pipegrid.matrix[current.y][current.x].tenyleges);
    bool is_crossed = c[0][1] != Levego && c[1][0] != Levego && c[1][2] != Levego && c[2][1] != Levego;
    
    if (is_crossed && c[1][0] == Ismeretlen && c[1][2] == Jo) {
        pipegrid.matrix[current.y][current.x].tenyleges[1][1] = Jo;
        return true;
    }
    else if (!is_crossed && pipegrid.matrix[current.y][current.x].tenyleges[1][2] == Jo) {
        pipegrid.matrix[current.y][current.x].tenyleges[1][1] = Jo;
        return true;
    }

    Irany iranyok[4] = {fel, le, jobbra, balra};

    for (int i = 0; i < 4; i++) {
        int r = rand() % (4 - i);
        Irany tmp = iranyok[i];
        iranyok[i] = iranyok[r];
        iranyok[r] = tmp;
    }
    
    for (int i = 0; i < 4; i++) {
        if (is_crossed) {
            if (c[0][1] == Ismeretlen && c[2][1] == Cso)
                iranyok[i] = le;
            else if (c[2][1] == Ismeretlen && c[0][1] == Cso)
                iranyok[i] = fel;
            else if (c[1][0] == Ismeretlen && c[1][2] == Cso)
                iranyok[i] = jobbra;
            else if (c[1][2] == Ismeretlen && c[1][0] == Cso)
                iranyok[i] = balra;
        }
        
        
        switch (iranyok[i]) {
            case fel:
                if (current.y > 0 && c[0][1] == Cso && pipegrid.matrix[current.y-1][current.x].tenyleges[2][1] == Cso) {
                    pipegrid.matrix[current.y][current.x].tenyleges[0][1] = Ismeretlen;
                    pipegrid.matrix[current.y-1][current.x].tenyleges[2][1] = Ismeretlen;

                    if (LabyrinthSolver(pipegrid, (SDL_Point){.x=current.x, .y=current.y-1})) {
                        pipegrid.matrix[current.y][current.x].tenyleges[1][1] = Jo;
                        pipegrid.matrix[current.y][current.x].tenyleges[0][1] = Jo;
                        pipegrid.matrix[current.y - 1][current.x].tenyleges[2][1] = Jo;
                        return true;
                    }
                    else {
                        pipegrid.matrix[current.y][current.x].tenyleges[0][1] = Rossz;
                        pipegrid.matrix[current.y - 1][current.x].tenyleges[2][1] = Rossz;
                    }
                }
                else if (c[0][1] == Cso) {
                    pipegrid.matrix[current.y][current.x].tenyleges[0][1] = Rossz;
                }
                break;

            case le:
                if (current.y < pipegrid.y - 1 && c[2][1] == Cso && pipegrid.matrix[current.y+1][current.x].tenyleges[0][1] == Cso) {
                    pipegrid.matrix[current.y][current.x].tenyleges[2][1] = Ismeretlen;
                    pipegrid.matrix[current.y+1][current.x].tenyleges[0][1] = Ismeretlen;
                    if (LabyrinthSolver(pipegrid, (SDL_Point){.x=current.x, .y=current.y+1})) {
                        pipegrid.matrix[current.y][current.x].tenyleges[1][1] = Jo;
                        pipegrid.matrix[current.y][current.x].tenyleges[2][1] = Jo;
                        pipegrid.matrix[current.y + 1][current.x].tenyleges[0][1] = Jo;
                        return true;
                    }
                    else {
                        pipegrid.matrix[current.y][current.x].tenyleges[2][1] = Rossz;
                        pipegrid.matrix[current.y + 1][current.x].tenyleges[0][1] = Rossz;
                    }
                }
                else if (c[2][1] == Cso) {
                    pipegrid.matrix[current.y][current.x].tenyleges[2][1] = Rossz;
                }
                break;

            case jobbra:
                if (current.x < pipegrid.x - 1 && c[1][2] == Cso && pipegrid.matrix[current.y][current.x+1].tenyleges[1][0] == Cso) {
                    pipegrid.matrix[current.y][current.x].tenyleges[1][2] = Ismeretlen;
                    pipegrid.matrix[current.y][current.x+1].tenyleges[1][0] = Ismeretlen;
                    if (LabyrinthSolver(pipegrid, (SDL_Point){.x=current.x+1, .y=current.y})) {
                        pipegrid.matrix[current.y][current.x].tenyleges[1][1] = Jo;
                        pipegrid.matrix[current.y][current.x].tenyleges[1][2] = Jo;
                        pipegrid.matrix[current.y][current.x + 1].tenyleges[1][0] = Jo;
                        return true;
                    }
                    else {
                        pipegrid.matrix[current.y][current.x].tenyleges[1][2] = Rossz;
                        pipegrid.matrix[current.y][current.x + 1].tenyleges[1][0] = Rossz;
                    }
                }
                else if (c[1][2] == Cso) {
                    pipegrid.matrix[current.y][current.x].tenyleges[1][2] = Rossz;
                }
                break;

            case balra:
                if (current.x > 0 && c[1][0] == Cso && pipegrid.matrix[current.y][current.x-1].tenyleges[1][2] == Cso) {
                    pipegrid.matrix[current.y][current.x].tenyleges[1][0] = Ismeretlen;
                    pipegrid.matrix[current.y][current.x-1].tenyleges[1][2] = Ismeretlen;
                    if (LabyrinthSolver(pipegrid, (SDL_Point){.x=current.x-1, .y=current.y})) {
                        pipegrid.matrix[current.y][current.x].tenyleges[1][1] = Jo;
                        pipegrid.matrix[current.y][current.x].tenyleges[1][0] = Jo;
                        pipegrid.matrix[current.y][current.x - 1].tenyleges[1][2] = Jo;
                        return true;
                    }
                    else {
                        pipegrid.matrix[current.y][current.x].tenyleges[1][0] = Rossz;
                        pipegrid.matrix[current.y][current.x - 1].tenyleges[1][2] = Rossz;
                    }
                }
                else if (c[1][0] == Cso) {
                    pipegrid.matrix[current.y][current.x].tenyleges[1][0] = Rossz;
                }
                break;
        }
    }

    pipegrid.matrix[current.y][current.x].tenyleges[1][1] = Rossz;

    return false;
}

bool Solve(PipeGrid pipegrid, SDL_Point entry, SDL_Point exit) {
    if (pipegrid.matrix[exit.y][exit.x].tenyleges[1][2] == Levego || pipegrid.matrix[entry.y][entry.x].tenyleges[1][0] == Levego) return false;
    pipegrid.matrix[entry.y][entry.x].tenyleges[1][0] = Ismeretlen;
    pipegrid.matrix[exit.y][exit.x].tenyleges[1][2] = Jo;

    bool bejart = LabyrinthSolver(pipegrid, entry);
    if (bejart) pipegrid.matrix[entry.y][entry.x].tenyleges[1][0] = Jo;

    return bejart;
}


SolvedPipeGrid * SolvedPipeGrid_Append(SolvedPipeGrid * start, Pipe * pipe, int x, int y) {
    if (start == NULL) {
        start = (SolvedPipeGrid*)malloc(sizeof(SolvedPipeGrid));
        start->pipe = pipe;
        start->x = x;
        start->y = y;
        start->next = NULL;
        return start;
    }

    SolvedPipeGrid * current = start;
    while (current->next != NULL)
        current = current->next;
    current->next = (SolvedPipeGrid*)malloc(sizeof(SolvedPipeGrid));
    current->next->pipe = pipe;
    current->next->x = x;
    current->next->y = y;
    current->next->next = NULL;

    return start;
}

SolvedPipeGrid * PipeGrid_Solve(PipeGrid pipegrid, SDL_Point entry, SDL_Point exit) {
    PipeGrid copy = PipeGrid_CreateCopy(pipegrid);
    bool solve = Solve(copy, entry, exit);
    if (!solve) {
        PipeGrid_Destroy(&copy);
        return NULL;
    } 
    SolvedPipeGrid * start = SolvedPipeGrid_Append(NULL, &pipegrid.matrix[entry.y][entry.x], entry.x, entry.y);
    SolvedPipeGrid * current = start;

    copy.matrix[entry.y][entry.x].tenyleges[1][0] = Rossz;

    while (current->x != exit.x || current->y != exit.y) {
        copy.matrix[current->y][current->x].tenyleges[1][1] = Rossz;
        Elem c;
        Elem_Copy(c, copy.matrix[current->y][current->x].tenyleges);
        bool is_crossed = c[0][1] != Levego && c[1][0] != Levego && c[1][2] != Levego && c[2][1] != Levego;

        if (is_crossed) {
            if (current->y > 0 && c[2][1] == Rossz && c[0][1] == Jo && copy.matrix[current->y - 1][current->x].tenyleges[2][1] == Jo) {
                copy.matrix[current->y][current->x].tenyleges[0][1] = Rossz;
                copy.matrix[current->y - 1][current->x].tenyleges[2][1] = Rossz;
                current = SolvedPipeGrid_Append(current, &pipegrid.matrix[current->y - 1][current->x], current->x, current->y - 1);
            }
            else if (current->y < copy.y - 1 && c[0][1] == Rossz && c[2][1] == Jo && copy.matrix[current->y + 1][current->x].tenyleges[0][1] == Jo) {
                copy.matrix[current->y][current->x].tenyleges[2][1] = Rossz;
                copy.matrix[current->y + 1][current->x].tenyleges[0][1] = Rossz;
                current = SolvedPipeGrid_Append(current, &pipegrid.matrix[current->y + 1][current->x], current->x, current->y + 1);
            }
            else if (current->x > 0 && c[1][2] == Rossz && c[1][0] == Jo && copy.matrix[current->y][current->x - 1].tenyleges[1][2] == Jo) {
                copy.matrix[current->y][current->x].tenyleges[1][0] = Rossz;
                copy.matrix[current->y][current->x - 1].tenyleges[1][2] = Rossz;
                current = SolvedPipeGrid_Append(current, &pipegrid.matrix[current->y][current->x - 1], current->x - 1, current->y);
            }
            else if (current->x < copy.x - 1 && c[1][0] == Rossz && c[1][2] == Jo && copy.matrix[current->y][current->x + 1].tenyleges[1][0] == Jo) {
                copy.matrix[current->y][current->x].tenyleges[1][2] = Rossz;
                copy.matrix[current->y][current->x + 1].tenyleges[1][0] = Rossz;
                current = SolvedPipeGrid_Append(current, &pipegrid.matrix[current->y][current->x+1], current->x + 1, current->y);
            }
        }
        else {
            if (current->y > 0 && c[0][1] == Jo && copy.matrix[current->y - 1][current->x].tenyleges[2][1] == Jo) {
                copy.matrix[current->y][current->x].tenyleges[0][1] = Rossz;
                copy.matrix[current->y - 1][current->x].tenyleges[2][1] = Rossz;
                current = SolvedPipeGrid_Append(current, &pipegrid.matrix[current->y - 1][current->x], current->x, current->y - 1);
            }
            else if (current->y < copy.y - 1 && c[2][1] == Jo && copy.matrix[current->y + 1][current->x].tenyleges[0][1] == Jo) {
                copy.matrix[current->y][current->x].tenyleges[2][1] = Rossz;
                copy.matrix[current->y + 1][current->x].tenyleges[0][1] = Rossz;
                current = SolvedPipeGrid_Append(current, &pipegrid.matrix[current->y+1][current->x], current->x, current->y + 1);
            }
            else if (current->x > 0 && c[1][0] == Jo && copy.matrix[current->y][current->x - 1].tenyleges[1][2] == Jo) {
                copy.matrix[current->y][current->x].tenyleges[1][0] = Rossz;
                copy.matrix[current->y][current->x - 1].tenyleges[1][2] = Rossz;
                current = SolvedPipeGrid_Append(current, &pipegrid.matrix[current->y][current->x-1], current->x - 1, current->y);
            }
            else if (current->x < copy.x - 1 && c[1][2] == Jo && copy.matrix[current->y][current->x + 1].tenyleges[1][0] == Jo) {
                copy.matrix[current->y][current->x].tenyleges[1][2] = Rossz;
                copy.matrix[current->y][current->x + 1].tenyleges[1][0] = Rossz;
                current = SolvedPipeGrid_Append(current, &pipegrid.matrix[current->y][current->x+1], current->x + 1, current->y);
            }
        }

        current = current->next;
    }

    current = SolvedPipeGrid_Append(current, &pipegrid.matrix[exit.y][exit.x], exit.x, exit.y);

    PipeGrid_Destroy(&copy);
    return start;
}

SolvedPipeGrid * SolvedPipeGrid_RemoveFirst(SolvedPipeGrid * start) {
    if (start == NULL) return NULL;
    SolvedPipeGrid * next = start->next;
    free(start);
    return next;
}

void SolvedPipeGrid_Destroy(SolvedPipeGrid * start) {
    while (start != NULL) {
        SolvedPipeGrid * tmp = start->next;
        free(start);
        start = tmp;
    }
}
