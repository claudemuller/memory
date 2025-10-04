#include "input.h"
#include <SDL3/SDL.h>

static void input_process_mouse(Input* input, SDL_Event* ev);
static void input_process_keyboard(Input* input, SDL_Event* ev);
static void input_process_gamepad(Input* input, SDL_Event* ev);

void input_process(Input* input, SDL_Event* ev)
{
    input_process_mouse(input, ev);
    input_process_keyboard(input, ev);
    input_process_gamepad(input, ev);
}

bool input_is_key_pressed(Input* input, KeyboardButtons btn)
{
    return IS_SET(input->kb.btns, (1U << btn));
}

bool input_is_mouse_btn_pressed(Input* input, MouseButtons btn)
{
    return IS_SET(input->mouse.btns, (1U << btn));
}

bool input_is_gamepad_btn_pressed(Input* input, GamepadButtons btn)
{
    return IS_SET(input->gamepad.btns, (1U << btn));
}

// ------------------------------------------------------------------------------------------------

static void input_process_mouse(Input* input, SDL_Event* ev)
{
}

static void input_process_keyboard(Input* input, SDL_Event* ev)
{
    if (ev->type == SDL_EVENT_KEY_DOWN) {
        switch (ev->key.key) {
        case SDLK_ESCAPE:
        case SDLK_Q: {
            input->kb.btns |= (1U << KB_KEY_Q);
        } break;

        case SDLK_SPACE: {
            input->kb.btns |= (1U << KB_KEY_SPACE);
        } break;

        case SDLK_UP: {
            input->kb.btns |= (1U << KB_KEY_UP);
        } break;

        case SDLK_LEFT: {
            input->kb.btns |= (1U << KB_KEY_LEFT);
        } break;

        case SDLK_DOWN: {
            input->kb.btns |= (1U << KB_KEY_DOWN);
        } break;

        case SDLK_RIGHT: {
            input->kb.btns |= (1U << KB_KEY_RIGHT);
        } break;
        }
    }

    if (ev->type == SDL_EVENT_KEY_UP) {
        switch (ev->key.key) {
        case SDLK_ESCAPE:
        case SDLK_Q: {
            input->kb.btns &= ~(1U << KB_KEY_Q);
        } break;

        case SDLK_SPACE: {
            input->kb.btns &= ~(1U << KB_KEY_SPACE);
        } break;

        case SDLK_UP: {
            input->kb.btns &= ~(1U << KB_KEY_UP);
        } break;

        case SDLK_LEFT: {
            input->kb.btns &= ~(1U << KB_KEY_LEFT);
        } break;

        case SDLK_DOWN: {
            input->kb.btns &= ~(1U << KB_KEY_DOWN);
        } break;

        case SDLK_RIGHT: {
            input->kb.btns &= ~(1U << KB_KEY_RIGHT);
        } break;
        }
    }
}

static void input_process_gamepad(Input* input, SDL_Event* ev)
{
}
