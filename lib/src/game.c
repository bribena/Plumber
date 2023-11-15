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
    SDL_Point prevCursor;

    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_QUIT) break;
        
        bool game = false;
        PipeGrid pg = PipeGrid_Init();
        int entry_y = 0;
        int exit_y = 0;

        // Katt
        if (!clicked && event.type == SDL_MOUSEBUTTONDOWN) {
            prevCursor = (SDL_Point){event.button.x, event.button.y};
            clicked = true;
        }

        if (clicked && event.type == SDL_MOUSEBUTTONUP) {
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

    SDL_Rect tmp_loc = {0, 0, x * DEFAULT_PIPE_SIZE, y * DEFAULT_PIPE_SIZE};
    PipeGrid_Feldolgoz(pipegrid, game_struct.window, game_struct.assets.pipes_empty, tmp_loc);

    tmp_loc.x = (game_struct.window.width - tmp_loc.w) / 2.0;
    tmp_loc.y = (game_struct.window.height - tmp_loc.h) / 2.0;

    Layer playfield = Layer_Init();
    Layer_Create(&playfield, game_struct.window, &tmp_loc);
    SDL_SetRenderTarget(game_struct.window.renderer, playfield.layer);
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
    SDL_SetRenderDrawColor(game_struct.window.renderer, 50, 50, 50, 200);

    SDL_RenderCopy(game_struct.window.renderer, background.layer, NULL, NULL);

    SDL_RenderFillRect(game_struct.window.renderer, &playfield.location);
    SDL_RenderCopy(game_struct.window.renderer, playfield.layer, NULL, &playfield.location);
    SDL_RenderCopy(game_struct.window.renderer, wheel.layer.layer, NULL, &wheel.layer.location);
    
    SDL_RenderPresent(game_struct.window.renderer);

    EventloopVariables ev_vars = EventloopVariables_Init();
    
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
                if (CursorInRect(ev_vars, playfield.location)) {
                    SDL_Point currCursor = {ev_vars.event.button.x, ev_vars.event.button.y};

                    int idx_x = (currCursor.x - playfield.location.x) / DEFAULT_PIPE_SIZE;
                    int idx_y = (currCursor.y - playfield.location.y) / DEFAULT_PIPE_SIZE;
                    int prev_x = (ev_vars.prevCursor.x - playfield.location.x) / DEFAULT_PIPE_SIZE;
                    int prev_y = (ev_vars.prevCursor.y - playfield.location.y) / DEFAULT_PIPE_SIZE;

                    if (idx_x == prev_x && idx_y == prev_y) {
                        
                    }
                }

                if (CursorInRect(ev_vars, vissza_btn.location)) {
                    ev_vars.running = false;
                    break;
                }

                if (CursorInRect(ev_vars, mentes_btn.location)) {
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

                ev_vars.clicked = false;
            }
        }

        Delta_Update(&ev_vars);
    }


    Wheel_Destroy(&wheel);
    PipeGrid_Destroy(&pipegrid);
    Layer_Destroy(&long_pipe);
    Layer_Destroy(&playfield);
    Layer_Destroy(&mentes_btn);
    Layer_Destroy(&vissza_btn);
    Layer_Destroy(&background);

    return ev_vars.event.type != SDL_QUIT;
}