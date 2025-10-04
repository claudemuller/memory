#ifndef GAME_H_
#define GAME_H_

#include "arena.h"
#include "input.h"
#include <SDL3/SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 640
#define FPS 60
#define MILLISECS_PER_FRAME 1000 / FPS

#define MAX_MOVES 100
#define QUAD_RADIUS 200.0f

typedef void (*StateFn)(void);

typedef enum {
    QUAD_DOWN,
    QUAD_LEFT,
    QUAD_UP,
    QUAD_RIGHT,
    QUAD_COUNT,
} Quadrants;

typedef enum {
    STATE_MAIN_MENU,
    STATE_WIN_SCREEN,
    STATE_GAME_OVER_SCREEN,
    STATE_IN_GAME,
    STATE_IN_GAME_INPUT,
    STATE_COUNT,
} State;

typedef struct {
    StateFn render[STATE_COUNT];
    StateFn update[STATE_COUNT];
} StateFns;

typedef struct {
    u8 quads[MAX_MOVES];
    size_t n_quads;
} QuadStack;

typedef struct GameState {
    SDL_Window* window;
    SDL_Renderer* renderer;

    QuadStack quad_stack;
    bool input_quads[4];
    Timer quad_timer;
    u8 curr_level_diff;
    u8 curr_show_quad;
    Input input;
    State curr_state;
    u64 prev_frame_ms;
    bool is_running;
} GameState;

bool game_init(void);
bool game_run(void);
void game_destroy(void);

static void quad_push(QuadStack* quad_stack, u8 random_quad)
{
    quad_stack->quads[quad_stack->n_quads++] = random_quad;
}

static u8 quad_pop(QuadStack* quad_stack)
{
    return quad_stack->quads[quad_stack->n_quads--];
}

#endif // !GAME_H_
