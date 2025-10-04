#ifndef GFX_H_
#define GFX_H_

#include "arena.h"
#include <SDL3/SDL.h>

void render_sector(SDL_Renderer* renderer,
                   f32 cx,
                   f32 cy,
                   f32 r,
                   f32 start_angle,
                   f32 end_angle,
                   u16 segments,
                   SDL_FColor color);

#endif // GFX_H_
