#include "pipe.h"
#include "debugmalloc.h"

void Elem_Init(Elem elem) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            elem[i][j] = Levego;
}

void Elem_Copy(Elem hova, Elem honnan) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            hova[i][j] = honnan[i][j];
}

void Elem_RotatePos(Elem elem) {
    Elem new;
    Elem_Init(new);
    new[0][1] = elem[1][0];
    new[1][0] = elem[2][1];
    new[1][2] = elem[0][1];
    new[2][1] = elem[1][2];
    new[1][1] = elem[1][1];
    Elem_Copy(elem, new);
}

void Elem_RotateNeg(Elem elem) {
    Elem new;
    Elem_Init(new);
    new[0][1] = elem[1][2];
    new[1][0] = elem[0][1];
    new[1][2] = elem[2][1];
    new[2][1] = elem[1][0];
    new[1][1] = elem[1][1];
    Elem_Copy(elem, new);
}