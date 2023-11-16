#include "game.h"
#include "debugmalloc.h"


void ButtonRender(SDL_Renderer * renderer, Layer layer, int padding, int rounding, SDL_Color color) {
    roundedBoxRGBA(renderer, layer.location.x-padding, layer.location.y-padding, layer.location.x + layer.location.w+padding, layer.location.y + layer.location.h+padding, rounding, color.r, color.g, color.b, color.a);
    roundedRectangleRGBA(renderer, layer.location.x-padding, layer.location.y-padding, layer.location.x + layer.location.w+padding, layer.location.y + layer.location.h+padding, rounding, 0, 0, 0, 255);
    SDL_RenderCopy(renderer, layer.layer, NULL, &layer.location);
}

void PipeGrid_Save(PipeGrid pipegrid, int entry_y, int exit_y, const char * path) {
    FILE * file = fopen(path, "w");
    fprintf(file, "%d %d %d %d\n", pipegrid.x, pipegrid.y, entry_y, exit_y);

    for (int i = 0; i < pipegrid.y; i++) {
        for (int k = 0; k < 3; k++) {
            for (int j = 0; j < pipegrid.x; j++) {
                for (int l = 0; l < 3; l++) {
                    fputc(pipegrid.matrix[i][j].tenyleges[k][l], file);
                }
                fputc(' ', file);
            }
            fputc('\n', file);
        }
        fputc('\n', file);
    }

    fclose(file);
} 

void PipeGrid_Load(const char * path, PipeGrid * pipegrid, int * entry_y, int * exit_y) {
    int x, y;
    FILE * file = fopen(path, "r");
    fscanf(file, "%d %d %d %d\n", &x, &y, entry_y, exit_y);

    PipeGrid_Create(pipegrid, x, y);
    for (int i = 0; i < y; i++) {
        for (int k = 0; k < 3; k++) {
            for (int l = 0; l < x; l++) {
                for (int j = 0; j < 3; j++) {
                    pipegrid->matrix[i][l].tenyleges[k][j] = fgetc(file);
                }
                fgetc(file);
            }
            fgetc(file);
        }
        fgetc(file);
    }
    
    fclose(file);
}



bool Menu(GameStruct game_struct) {
    Layer menu = Layer_Init();
    Layer_Create(&menu, game_struct.window, NULL);
    
    SDL_SetRenderTarget(game_struct.window.renderer, menu.layer);
    SDL_RenderCopy(game_struct.window.renderer, game_struct.assets.background, NULL, NULL);

    Layer title = Layer_Init();
    Layer_Create(&title, game_struct.window, &(SDL_Rect){400, 0, 800, 300});
    Layer_RenderFont(&title, game_struct.window, game_struct.fonts.bold, "Plumber", (SDL_Color){0,0,0,255});
    SDL_RenderCopy(game_struct.window.renderer, title.layer, NULL, &title.location);
    Layer_Destroy(&title);

    Layer jatek_btn = Layer_Init();
    Layer_Create(&jatek_btn, game_struct.window, &(SDL_Rect){675, 350, 250, 125});
    Layer_RenderFont(&jatek_btn, game_struct.window, game_struct.fonts.bold, "Játék", (SDL_Color){80,80,80,255});

    Layer betolt_btn = Layer_Init();
    Layer_Create(&betolt_btn, game_struct.window, &(SDL_Rect){500, 550, 600, 125});
    Layer_RenderFont(&betolt_btn, game_struct.window, game_struct.fonts.bold, "Pálya betöltése", (SDL_Color){80,80,80,255});

    Layer kilep_btn = Layer_Init();
    Layer_Create(&kilep_btn, game_struct.window, &(SDL_Rect){650, 750, 300, 125});
    Layer_RenderFont(&kilep_btn, game_struct.window, game_struct.fonts.bold, "Kilépés", (SDL_Color){80,80,80,255});

    
    SDL_SetRenderTarget(game_struct.window.renderer, NULL);
    
    SDL_RenderCopy(game_struct.window.renderer, menu.layer, NULL, NULL);
    ButtonRender(game_struct.window.renderer, jatek_btn, 10, 30, (SDL_Color){81, 229, 239, 255});
    ButtonRender(game_struct.window.renderer, betolt_btn, 10, 30, (SDL_Color){2, 216, 233, 255});
    ButtonRender(game_struct.window.renderer, kilep_btn, 10, 30, (SDL_Color){1, 168, 180, 255});

    SDL_RenderPresent(game_struct.window.renderer);
    
    SDL_Event event;
    bool clicked = false;
    bool game = false;
    SDL_Point prevCursor;

    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) break;
        
        PipeGrid pg = PipeGrid_Init();
        int entry_y = 0;
        int exit_y = 0;

        // Katt
        if (!clicked && event.type == SDL_MOUSEBUTTONDOWN) {
            prevCursor = (SDL_Point){event.button.x, event.button.y};
            clicked = true;
        }

        if (clicked && event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
            SDL_Point currCursor = {event.button.x, event.button.y};
            
            game = SDL_PointInRect(&currCursor, &jatek_btn.location) && SDL_PointInRect(&prevCursor, &jatek_btn.location);

            if (SDL_PointInRect(&currCursor, &betolt_btn.location) && SDL_PointInRect(&prevCursor, &betolt_btn.location)) {
                sfd_Options ops = {
                    .title="Pálya betöltése",
                    .filter_name="Palya fajl",
                    .filter="*.plm",
                };
                const char * path = sfd_open_dialog(&ops);
                if (path != NULL) {
                    PipeGrid_Load(path, &pg, &entry_y, &exit_y);
                    game = true;
                }
            }
            
            if (SDL_PointInRect(&currCursor, &kilep_btn.location) && SDL_PointInRect(&prevCursor, &kilep_btn.location)) break;

            clicked = false; 
        }


        // Játék indítás
        if (game && Game(game_struct, (pg.matrix == NULL ? NULL : &pg), entry_y, exit_y)) {
            game = false;

            SDL_RenderClear(game_struct.window.renderer);
            SDL_SetRenderTarget(game_struct.window.renderer, NULL);

            SDL_RenderCopy(game_struct.window.renderer, menu.layer, NULL, NULL);
            ButtonRender(game_struct.window.renderer, jatek_btn, 10, 30, (SDL_Color){81, 229, 239, 255});
            ButtonRender(game_struct.window.renderer, betolt_btn, 10, 30, (SDL_Color){2, 216, 233, 255});
            ButtonRender(game_struct.window.renderer, kilep_btn, 10, 30, (SDL_Color){1, 168, 180, 255});

            SDL_RenderPresent(game_struct.window.renderer);
        }
    }

    Layer_Destroy(&kilep_btn);
    Layer_Destroy(&betolt_btn);
    Layer_Destroy(&jatek_btn);
    Layer_Destroy(&menu);

    return true;
}



void RotateAngle(AnimationQueue * element, double deltaT) {
    double deltaAngle = element->target_angle * deltaT * element->animation_speed;
    if (SDL_fabs(deltaAngle + element->rendered_angle) > SDL_fabs(element->target_angle)) deltaAngle = element->target_angle - element->rendered_angle;

    element->rendered_angle += deltaAngle;
    *element->angle += deltaAngle;

    element->active = SDL_fabs(element->rendered_angle) < SDL_fabs(element->target_angle);

    if (*element->angle >= 360)
        *element->angle -= 360;
        
    if (*element->angle <= -360)
        *element->angle += 360;
}

void Fill(AnimationQueue * element, double deltaT) {
    double texture_delta = SDL_floor(element->target_length * deltaT * element->animation_speed);

    switch (element->direction) {
        case AnimationDirection_ToRight:
            if (texture_delta + element->texture_location.x > element->target_coor) 
                texture_delta = element->target_coor - element->texture_location.x - element->texture_location.w;

            element->texture_location.x += element->texture_location.w;
            element->render_location.x += element->render_location.w;

            element->texture_location.w = texture_delta;
            element->render_location.w = texture_delta * element->ratio;

            element->active = element->texture_location.x + element->texture_location.w < element->target_coor;
            break;

        case AnimationDirection_ToLeft:
            if (element->texture_location.x - texture_delta < element->target_coor) 
                texture_delta = element->texture_location.x - element->target_coor;
            
            element->texture_location.x -= texture_delta;
            element->texture_location.w = texture_delta;

            element->render_location.x -= texture_delta * element->ratio;
            element->render_location.w = texture_delta * element->ratio;

            element->active = element->texture_location.x > element->target_coor;

            break;

        case AnimationDirection_ToDown:
            if (texture_delta + element->texture_location.y > element->target_coor) 
                texture_delta = element->target_coor - element->texture_location.y - element->texture_location.h;
            
            element->texture_location.y += element->texture_location.h;
            element->render_location.y += element->render_location.h;

            element->texture_location.h = texture_delta;
            element->render_location.h = texture_delta * element->ratio;

            element->active = element->texture_location.y + element->texture_location.h < element->target_coor;

            break;
        
        case AnimationDirection_ToUp:
            if (element->texture_location.y - texture_delta < element->target_coor) 
                texture_delta = element->texture_location.y - element->target_coor;
            
            element->texture_location.y -= texture_delta;
            element->texture_location.h = texture_delta;

            element->render_location.y -= texture_delta * element->ratio;
            element->render_location.h = texture_delta * element->ratio;

            element->active = element->texture_location.y > element->target_coor;

            break;
    }
}

bool Game(GameStruct game_struct, PipeGrid * betoltott, int entry_y, int exit_y) {
    SDL_SetRenderTarget(game_struct.window.renderer, NULL);
    SDL_RenderClear(game_struct.window.renderer);

    Layer background = Layer_Init();
    Layer_Create(&background, game_struct.window, NULL);
    SDL_SetRenderTarget(game_struct.window.renderer, background.layer);
    SDL_RenderCopy(game_struct.window.renderer, game_struct.assets.background, NULL, NULL);

    Layer vissza_btn = Layer_Init();
    Layer_Create(&vissza_btn, game_struct.window, &(SDL_Rect){5, 5, 80, 40});
    Layer_RenderFont(&vissza_btn, game_struct.window, game_struct.fonts.bold, "Vissza", (SDL_Color){80,80,80,255});
    ButtonRender(game_struct.window.renderer, vissza_btn, 5, 0, (SDL_Color){1, 168, 180, 255});

    Layer mentes_btn = Layer_Init();
    Layer_Create(&mentes_btn, game_struct.window, &(SDL_Rect){background.location.w - 101, background.location.h - 45, 96, 40});
    Layer_RenderFont(&mentes_btn, game_struct.window, game_struct.fonts.bold, "Mentés", (SDL_Color){80,80,80,255});
    ButtonRender(game_struct.window.renderer, mentes_btn, 5, 0, (SDL_Color){2, 216, 233, 255});

    PipeGrid pipegrid = PipeGrid_Init();
    int x, y;

    if (betoltott != NULL) {
        pipegrid = *betoltott;
        x = pipegrid.x;
        y = pipegrid.y;
    }
    else {
        x = MIN_X_SIZE + rand() % 9;
        y = MIN_Y_SIZE + rand() % 3;
        if (rand() % 2) {
            entry_y = rand() % 4;
            exit_y = y - rand() % 4 - 1;
        }
        else {
            entry_y = y - rand() % 4 - 1;
            exit_y = rand() % 4;
        }
        PipeGrid_Create(&pipegrid, x, y);
        PipeGrid_GenerateGrid(&pipegrid, (SDL_Point){0, entry_y}, (SDL_Point){x-1, exit_y});
        PipeGrid_Shuffle(pipegrid);
    }

    PipeGrid copy = PipeGrid_CreateCopy(pipegrid);

    SDL_Rect tmp_loc = {0, 0, x * DEFAULT_PIPE_SIZE, y * DEFAULT_PIPE_SIZE};
    PipeGrid_Feldolgoz(pipegrid, game_struct.window, game_struct.assets.pipes_empty, tmp_loc);

    tmp_loc.x = (game_struct.window.width - tmp_loc.w) / 2.0;
    tmp_loc.y = (game_struct.window.height - tmp_loc.h) / 2.0;

    Layer playfield = Layer_Init();
    Layer_Create(&playfield, game_struct.window, &tmp_loc);
    SDL_SetRenderTarget(game_struct.window.renderer, playfield.layer);
    SDL_SetRenderDrawColor(game_struct.window.renderer, 50, 50, 50, 200);
    SDL_RenderFillRect(game_struct.window.renderer, NULL);
    PipeGrid_Render(pipegrid, game_struct.window);


    SDL_Rect lp_loc = {0, playfield.location.y + entry_y * DEFAULT_PIPE_SIZE, playfield.location.x, DEFAULT_PIPE_SIZE};
    Layer long_pipe = Layer_Init();
    Layer_Create(&long_pipe, game_struct.window, &lp_loc);
    SDL_SetRenderTarget(game_struct.window.renderer, long_pipe.layer);

    SDL_Rect long_pipe_texture_loc = {0, 0, 56, 500};
    SDL_Rect long_pipe_relative_loc = {0, 0, (double)lp_loc.h / long_pipe_texture_loc.h * 56, lp_loc.h};
    SDL_RenderCopy(game_struct.window.renderer, game_struct.assets.pipes_empty, &long_pipe_texture_loc, &long_pipe_relative_loc);
    long_pipe_texture_loc.x = 500 - long_pipe_texture_loc.w;
    long_pipe_relative_loc.x = lp_loc.w - long_pipe_relative_loc.w;
    SDL_RenderCopy(game_struct.window.renderer, game_struct.assets.pipes_empty, &long_pipe_texture_loc, &long_pipe_relative_loc);
    long_pipe_texture_loc.x = long_pipe_texture_loc.w;
    long_pipe_texture_loc.w = 500 - 2 * long_pipe_texture_loc.w;
    long_pipe_relative_loc.x = long_pipe_relative_loc.w;
    long_pipe_relative_loc.w = lp_loc.w - 2 * long_pipe_relative_loc.w;
    SDL_RenderCopy(game_struct.window.renderer, game_struct.assets.pipes_empty, &long_pipe_texture_loc, &long_pipe_relative_loc);

    Layer filled_long_pipe = Layer_Init();
    Layer_Create(&filled_long_pipe, game_struct.window, &lp_loc);
    SDL_SetRenderTarget(game_struct.window.renderer, filled_long_pipe.layer);
    SDL_RenderCopy(game_struct.window.renderer, long_pipe.layer, NULL, NULL);
    SDL_SetTextureColorMod(filled_long_pipe.layer, 0, 160, 255);
    SDL_SetTextureAlphaMod(filled_long_pipe.layer, 160);

    SDL_SetRenderTarget(game_struct.window.renderer, background.layer);
    SDL_RenderCopy(game_struct.window.renderer, long_pipe.layer, NULL, &long_pipe.location);

    lp_loc.x += playfield.location.w + playfield.location.x;
    lp_loc.y = playfield.location.y + pipegrid.matrix[exit_y][pipegrid.x-1].rendered.location.y;
    SDL_RenderCopy(game_struct.window.renderer, long_pipe.layer, NULL, &lp_loc);


    SDL_Rect w_loc = {0, 0, DEFAULT_PIPE_SIZE * WHEEL_SIZE_MULTIPLIER, DEFAULT_PIPE_SIZE * WHEEL_SIZE_MULTIPLIER};
    w_loc.x = playfield.location.x - w_loc.w * 1.25;
    w_loc.y = pipegrid.matrix[entry_y][0].rendered.location.y - (w_loc.h - DEFAULT_PIPE_SIZE) / 2 + playfield.location.y;
    Wheel wheel = Wheel_Init();
    Wheel_Create(&wheel, game_struct.window, w_loc, game_struct.assets.wheel);


    SDL_SetRenderTarget(game_struct.window.renderer, NULL);
    SDL_SetRenderDrawBlendMode(game_struct.window.renderer, SDL_BLENDMODE_BLEND);
    
    SDL_RenderCopy(game_struct.window.renderer, background.layer, NULL, NULL);

    
    SDL_RenderCopy(game_struct.window.renderer, playfield.layer, NULL, &playfield.location);
    SDL_RenderCopy(game_struct.window.renderer, wheel.layer.layer, NULL, &wheel.layer.location);


    SDL_RenderPresent(game_struct.window.renderer);

    EventloopVariables ev_vars = EventloopVariables_Init();
    bool freeze = false;
    bool solved = false;
    AnimationQueue * aq = AnimationQueue_Init();
    SolvedPipeGrid * solved_grid = NULL;

    while (ev_vars.running) {
        while (SDL_PollEvent(&ev_vars.event)) {
            if (ev_vars.event.type == SDL_QUIT) {
                ev_vars.running = false;
                SDL_PushEvent(&ev_vars.event);
                break;
            } 

            if (!ev_vars.clicked && ev_vars.event.type == SDL_MOUSEBUTTONDOWN) {
                ev_vars.clicked = true;
                ev_vars.prevCursor = (SDL_Point){ev_vars.event.button.x, ev_vars.event.button.y};
            }

            if (ev_vars.clicked && ev_vars.event.type == SDL_MOUSEBUTTONUP) {
                if (!freeze && CursorInRect(ev_vars, playfield.location)) {
                    SDL_Point currCursor = {ev_vars.event.button.x, ev_vars.event.button.y};

                    int idx_x = (currCursor.x - playfield.location.x) / DEFAULT_PIPE_SIZE;
                    int idx_y = (currCursor.y - playfield.location.y) / DEFAULT_PIPE_SIZE;
                    int prev_x = (ev_vars.prevCursor.x - playfield.location.x) / DEFAULT_PIPE_SIZE;
                    int prev_y = (ev_vars.prevCursor.y - playfield.location.y) / DEFAULT_PIPE_SIZE;

                    if (idx_x == prev_x && idx_y == prev_y) {
                        if (pipegrid.matrix[idx_y][idx_x].type == PipeType_Crossing) continue;
                        int dir = ev_vars.event.button.button == SDL_BUTTON_LEFT ? 1 : ev_vars.event.button.button == SDL_BUTTON_RIGHT ? -1 : 0;
                        if (dir != 0) {
                            Pipe_Rotate(&pipegrid.matrix[idx_y][idx_x], dir);
                            AnimationQueue_AppendPipeRotate(aq, &pipegrid.matrix[idx_y][idx_x], dir * 90, PIPE_ANIMATION_SPEED);
                        }
                    }
                }

                if (CursorInRect(ev_vars, vissza_btn.location)) {
                    ev_vars.running = false;
                    break;
                }

                if (!freeze && CursorInRect(ev_vars, mentes_btn.location)) {
                    sfd_Options ops = {
                        .title="Pálya mentése",
                        .filter_name="Palya fajl",
                        .filter="*.plm",
                    };
                    const char * path = sfd_save_dialog(&ops);
                    if (path != NULL) {
                        PipeGrid_Save(pipegrid, entry_y, exit_y, path);
                    }
                }

                if (!freeze && CursorInRect(ev_vars, wheel.layer.location)) {
                    int dir = ev_vars.event.button.button == SDL_BUTTON_LEFT ? 1 : ev_vars.event.button.button == SDL_BUTTON_RIGHT ? -1 : 0;
                    if (dir != 0) {
                        AnimationQueue_AppendWheelRotate(aq, &wheel, dir * 90, WHEEL_ANIMATION_SPEED);
                        if (dir == 1) {
                            AnimationQueue_AppendLongFill(aq, filled_long_pipe.location, filled_long_pipe.layer, LONG_PIPE_ANIMATION_SPEED, AnimationDirection_ToRight);
                            freeze = true;

                            solved_grid = PipeGrid_Solve(pipegrid, (SDL_Point){0, entry_y}, (SDL_Point){pipegrid.x-1, exit_y});
                            solved = solved_grid != NULL;
                        }
                    }
                }

                ev_vars.clicked = false;
            }
        }

        Delta_Update(&ev_vars);

        AnimationQueue * current = aq->next;
        while (current != NULL) {
            if (current->type == AnimationType_Rotate) {
                RotateAngle(current, ev_vars.deltaT);

                if (current->is_pipe) {
                    Pipe_UpdateLayer(*current->pipe, game_struct.window, game_struct.assets.pipes_empty);
                    SDL_SetRenderTarget(game_struct.window.renderer, playfield.layer);
                    SDL_SetRenderDrawBlendMode(game_struct.window.renderer, SDL_BLENDMODE_NONE);
                    SDL_SetRenderDrawColor(game_struct.window.renderer, 50, 50, 50, 200);
                    SDL_RenderFillRect(game_struct.window.renderer, &current->pipe->rendered.location);
                    SDL_SetRenderDrawBlendMode(game_struct.window.renderer, SDL_BLENDMODE_BLEND);
                    Pipe_Render(*current->pipe, game_struct.window);
                }
                else {
                    Wheel_Update(*current->wheel, game_struct.window, game_struct.assets.wheel);
                }
            }
            else {
                if (current->is_pipe) {
                    Fill(current, ev_vars.deltaT);
                    SDL_SetRenderTarget(game_struct.window.renderer, playfield.layer);
                    SDL_RenderCopy(game_struct.window.renderer, current->filled_texture, &current->texture_location, &current->render_location);
                    if (!current->active) {
                        if (solved_grid->next != NULL) {
                            AnimationDirection direction;
                        
                            if (solved_grid->x < solved_grid->next->x)
                                direction = AnimationDirection_ToRight;
                            else if (solved_grid->x > solved_grid->next->x)
                                direction = AnimationDirection_ToLeft;
                            else if (solved_grid->y < solved_grid->next->y) 
                                direction = AnimationDirection_ToDown;
                            else 
                                direction = AnimationDirection_ToUp;

                            AnimationQueue_AppendPipeFill(aq, game_struct.window.renderer, solved_grid->pipe, game_struct.assets.pipes_full, PIPE_FILL_ANIMATION_SPEED, direction);
                        }
                        else {
                            AnimationQueue_AppendLongFill(aq, lp_loc, filled_long_pipe.layer, LONG_PIPE_ANIMATION_SPEED, AnimationDirection_ToRight);
                        }
                        solved_grid = SolvedPipeGrid_RemoveFirst(solved_grid);
                    }
                }
                else {
                    Fill(current, ev_vars.deltaT);
                    SDL_SetRenderTarget(game_struct.window.renderer, background.layer);
                    SDL_RenderCopy(game_struct.window.renderer, current->filled_texture, &current->texture_location, &current->render_location);
                    if (!current->active && !solved && current->direction == AnimationDirection_ToRight) {
                        AnimationQueue_AppendLongFill(aq, long_pipe.location, long_pipe.layer, LONG_PIPE_ANIMATION_SPEED, AnimationDirection_ToLeft);
                    }
                    else if (!current->active && !solved && current->direction == AnimationDirection_ToLeft) freeze = false;
                    else if (!current->active && solved_grid != NULL) {
                        AnimationQueue_AppendPipeFill(aq, game_struct.window.renderer, solved_grid->pipe, game_struct.assets.pipes_full, PIPE_FILL_ANIMATION_SPEED, AnimationDirection_ToRight);
                    }
                }
            }

            SDL_SetRenderTarget(game_struct.window.renderer, NULL);
            SDL_RenderCopy(game_struct.window.renderer, background.layer, NULL, NULL);
            SDL_RenderCopy(game_struct.window.renderer, playfield.layer, NULL, &playfield.location);
            SDL_RenderCopy(game_struct.window.renderer, wheel.layer.layer, NULL, &wheel.layer.location);
            SDL_RenderPresent(game_struct.window.renderer);

            if (!current->active)
                current = AnimationQueue_RemoveElement(aq, current);
            current = current->next;
        }

        if (solved && solved_grid == NULL && aq->next == NULL) break;
    }

    Layer state = Layer_Init();
    Layer_Create(&state, game_struct.window, NULL);
    SDL_SetRenderTarget(game_struct.window.renderer, state.layer);
    SDL_RenderCopy(game_struct.window.renderer, background.layer, NULL, NULL);
    SDL_RenderCopy(game_struct.window.renderer, playfield.layer, NULL, &playfield.location);
    SDL_RenderCopy(game_struct.window.renderer, wheel.layer.layer, NULL, &wheel.layer.location);

    bool win = solved && Win(game_struct, state, copy, entry_y, exit_y);

    SolvedPipeGrid_Destroy(solved_grid);
    AnimationQueue_Destroy(aq);
    Wheel_Destroy(&wheel);
    PipeGrid_Destroy(&copy);
    PipeGrid_Destroy(&pipegrid);
    Layer_Destroy(&state);
    Layer_Destroy(&filled_long_pipe);
    Layer_Destroy(&long_pipe);
    Layer_Destroy(&playfield);
    Layer_Destroy(&mentes_btn);
    Layer_Destroy(&vissza_btn);
    Layer_Destroy(&background);

    return ev_vars.event.type != SDL_QUIT && (win || !solved);
}

bool Win(GameStruct game_struct, Layer state, PipeGrid copy, int entry_y, int exit_y) {
    SDL_SetRenderTarget(game_struct.window.renderer, NULL);
    SDL_RenderClear(game_struct.window.renderer);
    
    Layer win = Layer_Init();
    Layer_Create(&win, game_struct.window, NULL);
    SDL_SetRenderTarget(game_struct.window.renderer, win.layer);

    SDL_RenderCopy(game_struct.window.renderer, state.layer, NULL, NULL);

    SDL_SetRenderDrawBlendMode(game_struct.window.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(game_struct.window.renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(game_struct.window.renderer, NULL);

    SDL_Rect pr = {400, 275, 800, 450};
    SDL_SetRenderDrawColor(game_struct.window.renderer, 192, 154, 107, 255);
    SDL_RenderFillRect(game_struct.window.renderer, &pr);
    SDL_SetRenderDrawColor(game_struct.window.renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(game_struct.window.renderer, &pr);

    Layer congrats = Layer_Init();
    Layer_Create(&congrats, game_struct.window, &(SDL_Rect){500, 275, 600, 150});
    Layer_RenderFont(&congrats, game_struct.window, game_struct.fonts.bold, "Gratulálok!", (SDL_Color){0,0,0,255});
    SDL_RenderCopy(game_struct.window.renderer, congrats.layer, NULL, &congrats.location);

    Layer line1 = Layer_Init();
    Layer_Create(&line1, game_struct.window, &(SDL_Rect){410, 425, 780, 80});
    Layer_RenderFont(&line1, game_struct.window, game_struct.fonts.bold, "Szeretnél menteni, új játékot", (SDL_Color){0,0,0,255});
    SDL_RenderCopy(game_struct.window.renderer, line1.layer, NULL, &line1.location);

    Layer line2 = Layer_Init();
    Layer_Create(&line2, game_struct.window, &(SDL_Rect){410, 525, 780, 80});
    Layer_RenderFont(&line2, game_struct.window, game_struct.fonts.bold, "kezdeni vagy a menübe lépni?", (SDL_Color){0,0,0,255});
    SDL_RenderCopy(game_struct.window.renderer, line2.layer, NULL, &line2.location);

    Layer menu_btn = Layer_Init();
    Layer_Create(&menu_btn, game_struct.window, &(SDL_Rect){450, 640, 100, 50});
    Layer_RenderFont(&menu_btn, game_struct.window, game_struct.fonts.bold, "Menü", (SDL_Color){80,80,80,255});
    ButtonRender(game_struct.window.renderer, menu_btn, 10, 30, (SDL_Color){1, 168, 180, 255});
    
    Layer jatek_btn = Layer_Init();
    Layer_Create(&jatek_btn, game_struct.window, &(SDL_Rect){725, 640, 130, 50});
    Layer_RenderFont(&jatek_btn, game_struct.window, game_struct.fonts.bold, "Új játék", (SDL_Color){80,80,80,255});
    ButtonRender(game_struct.window.renderer, jatek_btn, 10, 30, (SDL_Color){81, 229, 239, 255});

    Layer mentes_btn = Layer_Init();
    Layer_Create(&mentes_btn, game_struct.window, &(SDL_Rect){1030, 640, 110, 50});
    Layer_RenderFont(&mentes_btn, game_struct.window, game_struct.fonts.bold, "Mentés", (SDL_Color){80,80,80,255});
    ButtonRender(game_struct.window.renderer, mentes_btn, 10, 30, (SDL_Color){2, 216, 233, 255});

    SDL_SetRenderTarget(game_struct.window.renderer, NULL);
    SDL_RenderCopy(game_struct.window.renderer, win.layer, NULL, NULL);
    SDL_RenderPresent(game_struct.window.renderer);

    SDL_Event event;
    SDL_Point prevCursor;
    bool clicked = false;
    bool menu = false;

    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) {
            menu = false;
            SDL_PushEvent(&event);
            break;
        }

        if (!clicked && event.type == SDL_MOUSEBUTTONDOWN) {
            clicked = true;
            prevCursor = (SDL_Point){event.button.x, event.button.y};
        }

        if (clicked && event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
            SDL_Point currCursor = {event.button.x, event.button.y};

            if (SDL_PointInRect(&currCursor, &menu_btn.location) && SDL_PointInRect(&prevCursor, &menu_btn.location)) {
                menu = true;
                break;
            }

            if (SDL_PointInRect(&currCursor, &jatek_btn.location) && SDL_PointInRect(&prevCursor, &jatek_btn.location)) {
                menu = false;
                break;
            }

            if (SDL_PointInRect(&currCursor, &mentes_btn.location) && SDL_PointInRect(&prevCursor, &mentes_btn.location)) {
                sfd_Options ops = {
                    .title="Pálya mentése",
                    .filter_name="Palya fajl",
                    .filter="*.plm",
                };
                const char * path = sfd_save_dialog(&ops);
                if (path != NULL) {
                    PipeGrid_Save(copy, entry_y, exit_y, path);
                }
            }

            clicked = false;
        }
    }

    Layer_Destroy(&mentes_btn);
    Layer_Destroy(&jatek_btn);
    Layer_Destroy(&menu_btn);
    Layer_Destroy(&line2);
    Layer_Destroy(&line1);
    Layer_Destroy(&congrats);
    Layer_Destroy(&win);
    return menu;
}