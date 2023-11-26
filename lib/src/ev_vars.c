/**
 * @file ev_vars.c
 * @brief A game.h-ban deklarált EventloopVariables struktúrához és az eventloophoz kapcsolódó függvények kódjait
 * tartalmazza a fájl.
 */

#include "game.h"
#include "debugmalloc.h"


EventloopVariables EventloopVariables_Init(void) {
    EventloopVariables ev_vars;
    ev_vars.clicked = false;
    ev_vars.currTime = 0;
    ev_vars.deltaT = 0;
    ev_vars.prevCursor = (SDL_Point){0, 0};
    ev_vars.prevTime = 0;
    ev_vars.running = true;
    return ev_vars;
}

void Delta_Update(EventloopVariables * ev_vars) {
    ev_vars->prevTime = ev_vars->currTime;
    ev_vars->currTime = SDL_GetPerformanceCounter();
    ev_vars->deltaT = ((ev_vars->currTime - ev_vars->prevTime) / (double)SDL_GetPerformanceFrequency());
}

bool CursorInRect(EventloopVariables ev_vars, SDL_Rect bound) {
    SDL_Point current = {ev_vars.event.button.x, ev_vars.event.button.y};
    return SDL_PointInRect(&current, &bound) && SDL_PointInRect(&ev_vars.prevCursor, &bound);
}