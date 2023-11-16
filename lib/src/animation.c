#include "game.h"
#include "debugmalloc.h"

AnimationQueue * AnimationQueue_Init(void) {
    AnimationQueue * new = (AnimationQueue*)malloc(sizeof(AnimationQueue));
    new->active = false;
    new->next = NULL;
    new->rendered_angle = 0;
    new->pipe = NULL;
    new->wheel = NULL;
    return new;
}

AnimationQueue * AnimationQueue_NewRotate(double * angle, double target_angle, double animation_speed) {
    AnimationQueue * new = AnimationQueue_Init();
    new->active = true;
    new->type = AnimationType_Rotate;
    new->angle = angle;
    new->target_angle = target_angle;
    new->animation_speed = animation_speed;
    return new;
}

void AnimationQueue_AppendWheelRotate(AnimationQueue * start, Wheel * wheel, double target_angle, double animation_speed) {
    AnimationQueue * current = start;
    while (current->next != NULL)
        current = current->next;
    
    current->next = AnimationQueue_NewRotate(&wheel->szog, target_angle, animation_speed);
    current = current->next;

    current->is_pipe = false;
    current->wheel = wheel;
}

void AnimationQueue_AppendPipeRotate(AnimationQueue * start, Pipe * pipe, double target_angle, double animation_speed) {
    AnimationQueue * current = start;
    while (current->next != NULL)
        current = current->next;
    
    current->next = AnimationQueue_NewRotate(&pipe->szog, target_angle, animation_speed);
    current = current->next;

    current->is_pipe = true;
    current->pipe = pipe;
}

void AnimationQueue_AppendLongFill(AnimationQueue * start, SDL_Rect render_location, SDL_Texture * filled_texture, double animation_speed, AnimationDirection direction) {
    AnimationQueue * current = start;
    while (current->next != NULL)
        current = current->next;
    
    current->next = AnimationQueue_Init();
    current = current->next;

    current->active = true;
    current->type = AnimationType_Fill;
    current->is_pipe = false;
    
    current->render_location = render_location;
    current->texture_location = render_location;
    current->ratio = current->render_location.w / (double)current->texture_location.w;

    current->texture_location.x = 0;
    current->texture_location.y = 0;

    current->target_length = current->texture_location.w;

    if (direction == AnimationDirection_ToRight) {
        current->target_coor = current->texture_location.w + current->texture_location.x;
        current->render_location.w = 0;
        current->texture_location.w = 0;
    }
    else if (direction == AnimationDirection_ToLeft) {
        current->target_coor = current->texture_location.x;
        current->texture_location.x += current->texture_location.w;
        current->texture_location.w = 0;
        current->render_location.x += current->render_location.w;
        current->render_location.w = 0;
    }

    current->filled_texture = filled_texture;
    current->animation_speed = animation_speed;
    current->direction = direction;
}

void AnimationQueue_AppendPipeFill(AnimationQueue * start, SDL_Renderer * renderer, Pipe * pipe, SDL_Texture * filled_texture, double animation_speed, AnimationDirection direction) {
    AnimationQueue * current = start;
    while (current->next != NULL)
        current = current->next;
    
    current->next = AnimationQueue_Init();
    current = current->next;

    current->active = true;
    current->type = AnimationType_Fill;
    current->is_pipe = true;
    current->pipe = pipe;
    current->animation_speed = animation_speed;
    current->direction = direction;

    SDL_SetRenderTarget(renderer, pipe->rendered.layer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderCopyEx(renderer, filled_texture, &pipe->texture_loc, NULL, pipe->szog, NULL, SDL_FLIP_NONE);
    SDL_SetRenderTarget(renderer, NULL);

    current->filled_texture = pipe->rendered.layer;

    current->render_location = pipe->rendered.location;
    current->texture_location = pipe->rendered.location;
    current->texture_location.y = 0;
    current->texture_location.x = 0;
    current->ratio = current->render_location.w / (double)current->texture_location.w;

    current->target_length = current->texture_location.w;

    switch (direction) {
        case AnimationDirection_ToRight:
            current->target_coor = current->texture_location.w + current->texture_location.x;
            current->texture_location.w = 0;
            current->render_location.w = 0;
            break;
        case AnimationDirection_ToLeft:
            current->target_coor = current->texture_location.x;
            current->texture_location.x += current->texture_location.w;
            current->texture_location.w = 0;
            current->render_location.x += current->render_location.w;
            current->render_location.w = 0;
            break;

        case AnimationDirection_ToDown:
            current->target_coor = current->texture_location.h + current->texture_location.y;
            current->texture_location.h = 0;
            current->render_location.h = 0;
            break;
        case AnimationDirection_ToUp:
            current->target_coor = current->texture_location.y;
            current->texture_location.y += current->texture_location.h;
            current->texture_location.h = 0;
            current->render_location.y += current->render_location.h;
            current->render_location.h = 0;
            break;
    }
}

AnimationQueue * AnimationQueue_RemoveElement(AnimationQueue * start, AnimationQueue * element) {
    AnimationQueue * current = start;
    while (current->next != element)
        current = current->next;
    
    current->next = element->next;
    free(element);
    return current;
}

void AnimationQueue_Destroy(AnimationQueue * start) {
    while (start != NULL) {
        AnimationQueue * tmp = start->next;
        free(start);
        start = tmp;
    }
}