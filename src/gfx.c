#include "gfx.h"
#include "arena.h"
#include "utils.h"
#include <SDL3/SDL_render.h>
#include <stddef.h>

void render_sector(SDL_Renderer* renderer,
                   f32 cx,
                   f32 cy,
                   f32 r,
                   f32 start_angle,
                   f32 end_angle,
                   u16 segments,
                   SDL_FColor colour)
{
    MemoryArena mem;

    u16 nindices = segments * 3;
    u16 nverts = 1 + (segments + 1);
    size_t verts_size = sizeof(SDL_Vertex) * nverts;
    size_t indices_size = sizeof(int) * nindices;

    arena_init(&mem, verts_size + indices_size);
    {
        // Number of vertices: center + (segments+1) arc points
        SDL_Vertex* verts = (SDL_Vertex*)arena_alloc_aligned(&mem, verts_size, 16);
        if (!verts) {
            util_err("no mem for verts");
            return;
        };

        // Center vertex
        verts[0].position.x = cx;
        verts[0].position.y = cy;
        verts[0].color = colour;
        verts[0].tex_coord.x = 0.0f;
        verts[0].tex_coord.y = 0.0f;

        // Arc vertices
        for (int i = 0; i <= segments; ++i) {
            f32 t = (float)i / (float)segments;
            f32 angle = start_angle + t * (end_angle - start_angle);
            f32 x = cx + r * cosf(angle);
            f32 y = cy + r * sinf(angle);

            verts[1 + i].position.x = x;
            verts[1 + i].position.y = y;
            verts[1 + i].color = colour;
            verts[1 + i].tex_coord.x = 0.0f;
            verts[1 + i].tex_coord.y = 0.0f;
        }

        // Build indices for triangles (triangles = segments)
        int* indices = (int*)arena_alloc_aligned(&mem, indices_size, 16);
        if (!indices) {
            util_err("no mem for indices");
            return;
        }

        int idx = 0;
        for (int i = 0; i < segments; ++i) {
            indices[idx++] = 0;
            indices[idx++] = 1 + i;
            indices[idx++] = 1 + i + 1;
        }

        SDL_RenderGeometry(renderer, NULL, verts, nverts, indices, nindices);
    }
    arena_free(&mem);
}
