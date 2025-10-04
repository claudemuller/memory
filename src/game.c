#include "game.h"
#include "gfx.h"
#include "input.h"
#include "utils.h"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static void process_events(void);
static void load_level(const u8 level);
static void update(const f64 dt);
static void render(void);
static void update_main_menu(void);
static void update_game_over_screen(void);
static void update_win_screen(void);
static void update_in_game(void);
static void render_main_menu(void);
static void render_game_over_screen(void);
static void render_win_screen(void);
static void render_in_game(void);

static GameState state;
static StateFns states;

bool game_init(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        util_error("Error init'ing SDL: %s", SDL_GetError());
        return false;
    }

    state.window = SDL_CreateWindow("DEV SDL3", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!state.window) {
        util_error("Error creating SDL window: %s", SDL_GetError());
        return false;
    }

    state.renderer = SDL_CreateRenderer(state.window, NULL);
    if (!state.renderer) {
        util_error("Error creating SDL renderer: %s", SDL_GetError());
        return false;
    }

    states.update[STATE_MAIN_MENU] = update_main_menu;
    states.render[STATE_MAIN_MENU] = render_main_menu;

    states.update[STATE_GAME_OVER_SCREEN] = update_main_menu;
    states.render[STATE_GAME_OVER_SCREEN] = render_game_over_screen;

    states.update[STATE_WIN_SCREEN] = update_main_menu;
    states.render[STATE_WIN_SCREEN] = render_win_screen;

    states.update[STATE_IN_GAME] = update_in_game;
    states.render[STATE_IN_GAME] = render_in_game;
    states.update[STATE_IN_GAME_INPUT] = update_in_game;
    states.render[STATE_IN_GAME_INPUT] = render_in_game;

    srand((u8)time(NULL));

    // state.prev_frame_ms = 0.0f;
    state.is_running = true;

    return true;
}

bool game_run(void)
{
    load_level(1);
    state.curr_state = STATE_MAIN_MENU;

    while (state.is_running) {
        u32 time_to_wait = MILLISECS_PER_FRAME - (SDL_GetTicks() - state.prev_frame_ms);
        if (time_to_wait > 0 && time_to_wait <= MILLISECS_PER_FRAME) {
            SDL_Delay(time_to_wait);
        }

        f64 dt = (SDL_GetTicks() - state.prev_frame_ms) / 1000.0;
        state.prev_frame_ms = SDL_GetTicks();

        process_events();
        update(dt);
        render();
    }

    return true;
}

void game_destroy(void)
{
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}

// ------------------------------------------------------------------------------------------------

static void process_events(void)
{
    input_clear(&state.input);

    SDL_Event ev;

    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_EVENT_QUIT) {
            state.is_running = false;
        }

        input_process(&state.input, &ev);
    }
}

static void load_level(const u8 level)
{
    memset(state.input_quads, false, sizeof(bool) * 4);

    state.curr_level_diff = level;

    for (size_t i = 0; i < level * QUAD_COUNT; ++i) {
        u8 random_quad = rand() % 4;
        quad_push(&state.quad_stack, i);
    }

    // TODO: debug
    for (size_t i = 0; i < state.quad_stack.n_quads; ++i) {
        util_info("%d ", state.quad_stack.quads[i]);
    }
}

static void update(const f64 dt)
{
    state.is_running = !input_is_key_pressed(&state.input, KB_KEY_Q);

    if (states.update[state.curr_state]) {
        states.update[state.curr_state]();
    }
}

static void render_debug_ui(void)
{
    char curr_state[20];
    switch (state.curr_state) {
    case STATE_MAIN_MENU: {
        snprintf(curr_state, 20, "main_menu");
    } break;
    case STATE_WIN_SCREEN: {
        snprintf(curr_state, 20, "win");
    } break;
    case STATE_GAME_OVER_SCREEN: {
        snprintf(curr_state, 20, "game_over");
    } break;
    case STATE_IN_GAME: {
        snprintf(curr_state, 20, "in_game");
    } break;
    case STATE_IN_GAME_INPUT: {
        snprintf(curr_state, 20, "in_game_input");
    } break;
    };

    SDL_RenderDebugText(state.renderer, 10.0f, 10.0f, curr_state);
}

static void render(void)
{
    SDL_SetRenderDrawColor(state.renderer, 0xaa, 0xb0, 0x78, 0xFF);
    SDL_RenderClear(state.renderer);

    if (states.render[state.curr_state]) {
        states.render[state.curr_state]();
    }

    render_debug_ui();

    SDL_RenderPresent(state.renderer);
}

static void update_main_menu(void)
{
    if (input_is_key_pressed(&state.input, KB_KEY_SPACE)) {
        state.curr_state = STATE_IN_GAME;
    }
    if (input_is_key_pressed(&state.input, KB_KEY_Q)) {
        state.is_running = false;
    }
}

static void update_in_game(void)
{
    if (state.curr_state != STATE_IN_GAME_INPUT) {
        timer_start(&state.quad_timer, SDL_GetTicks(), 2.0f * SECOND / state.curr_level_diff);

        // TODO: debug
        if (!timer_done(&state.quad_timer, SDL_GetTicks())) {
            // util_info("quad timer started - %d", state.curr_show_quad);
        }

        if (timer_done(&state.quad_timer, SDL_GetTicks())) {
            timer_stop(&state.quad_timer);
            // util_info("quad time done - %d", state.curr_show_quad);
            // util_info("quad time done - %d", quad_pop(&state.quad_stack));

            memset(state.input_quads, false, sizeof(bool) * 4);
            u8 q = state.quad_stack.quads[state.curr_show_quad++];
            util_info("%d", q);

            if (state.curr_show_quad > QUAD_COUNT) {
                state.curr_state = STATE_IN_GAME_INPUT;
                util_info("next state");
                return;
            }

            state.input_quads[q] = true;
        }

        return;
    }

    memset(state.input_quads, false, sizeof(bool) * 4);

    if (input_is_key_pressed(&state.input, KB_KEY_UP) ||
        input_is_gamepad_btn_pressed(&state.input, GAMEPAD_BTN_LEFT_UP)) {
        state.input_quads[QUAD_UP] = true;

        u8 q = quad_pop(&state.quad_stack);
        util_info("pressed: %d:%d", QUAD_UP, q);

        if (q != QUAD_UP) {
            // state.curr_state = STATE_GAME_OVER_SCREEN;
            return;
        }
    }
    if (input_is_key_pressed(&state.input, KB_KEY_RIGHT) ||
        input_is_gamepad_btn_pressed(&state.input, GAMEPAD_BTN_LEFT_RIGHT)) {
        state.input_quads[QUAD_RIGHT] = true;

        u8 q = quad_pop(&state.quad_stack);
        util_info("pressed: %d:%d", QUAD_RIGHT, q);

        if (q != QUAD_RIGHT) {
            // state.curr_state = STATE_GAME_OVER_SCREEN;
            return;
        }
    }
    if (input_is_key_pressed(&state.input, KB_KEY_DOWN) ||
        input_is_gamepad_btn_pressed(&state.input, GAMEPAD_BTN_LEFT_DOWN)) {
        state.input_quads[QUAD_DOWN] = true;

        u8 q = quad_pop(&state.quad_stack);
        util_info("pressed: %d:%d", QUAD_DOWN, q);

        if (q != QUAD_DOWN) {
            // state.curr_state = STATE_GAME_OVER_SCREEN;
            return;
        }
    }
    if (input_is_key_pressed(&state.input, KB_KEY_LEFT) ||
        input_is_gamepad_btn_pressed(&state.input, GAMEPAD_BTN_LEFT_LEFT)) {
        state.input_quads[QUAD_LEFT] = true;

        u8 q = quad_pop(&state.quad_stack);
        util_info("pressed: %d:%d", QUAD_LEFT, q);

        if (q != QUAD_LEFT) {
            // state.curr_state = STATE_GAME_OVER_SCREEN;
            return;
        }
    }
}

static void render_main_menu(void)
{
    SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderDebugText(state.renderer, 20.0f, 20.0f, "Main menu. Press <space> start");
}

static void render_game_over_screen(void)
{
    SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderDebugText(state.renderer, 20.0f, 20.0f, "Game over. Press <space> start again, or <escape> to quit");
}

static void render_win_screen(void)
{
    SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderDebugText(state.renderer, 20.0f, 20.0f, "You win. Press <space> start again, or <escape> to quit");
}

static void render_in_game(void)
{
    float cx = 400.0f, cy = 300.0f;
    float radius = 200.0f;
    u16 segsPerQuarter = 40;

    // colours for 4 slices
    SDL_FColor colours[4] = {
        {1.0f, 0.3f, 0.3f, 1.0f}, // red
        {0.3f, 1.0f, 0.3f, 1.0f}, // green
        {0.3f, 0.3f, 1.0f, 1.0f}, // blue
        {1.0f, 1.0f, 0.3f, 1.0f}  // yellow
    };
    // SDL_FColor hi_colours[4] = {
    //     {1.0f, 0.7f, 0.7f, 1.0f}, // soft light red
    //     {0.7f, 1.0f, 0.7f, 1.0f}, // soft light green
    //     {0.7f, 0.7f, 1.0f, 1.0f}, // soft light blue
    //     {1.0f, 1.0f, 0.7f, 1.0f}  // soft light yellow
    // };
    SDL_FColor hi_colours[4] = {
        {1.0f, 0.6f, 0.6f, 1.0f}, // lighter red
        {0.6f, 1.0f, 0.6f, 1.0f}, // lighter green
        {0.6f, 0.6f, 1.0f, 1.0f}, // lighter blue
        {1.0f, 1.0f, 0.6f, 1.0f}  // lighter yellow
    };

    for (size_t q = 0; q < QUAD_COUNT; ++q) {
        f32 start = (float)q * (M_PI / 2.0f);
        f32 end = (float)(q + 1) * (M_PI / 2.0f);

        if (state.input_quads[q]) {
            render_sector(state.renderer, cx, cy, radius, start, end, segsPerQuarter, hi_colours[q]);
        } else {
            render_sector(state.renderer, cx, cy, radius, start, end, segsPerQuarter, colours[q]);
        }
    }

    SDL_SetRenderDrawColor(state.renderer, 0x66, 0x66, 0x66, 200);

    u16 outlineSegs = 200;

    for (size_t i = 0; i < outlineSegs; ++i) {
        f32 a1 = (float)i / outlineSegs * 2.0f * M_PI;
        f32 a2 = (float)(i + 1) / outlineSegs * 2.0f * M_PI;

        i16 x1 = (int)(cx + radius * cosf(a1));
        i16 y1 = (int)(cy + radius * sinf(a1));
        i16 x2 = (int)(cx + radius * cosf(a2));
        i16 y2 = (int)(cy + radius * sinf(a2));

        SDL_RenderLine(state.renderer, x1, y1, x2, y2);
    }
}
