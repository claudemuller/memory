#ifndef INPUT_H_
#define INPUT_H_

#include "utils.h"
#include <SDL3/SDL.h>

typedef enum {
    MOUSE_BTN_LEFT,
    MOUSE_BTN_MIDDLE,
    MOUSE_BTN_RIGHT,
} MouseButtons;

typedef enum {
    KB_KEY_Q,
    KB_KEY_UP,
    KB_KEY_DOWN,
    KB_KEY_LEFT,
    KB_KEY_RIGHT,
    KB_KEY_SPACE,
    KB_KEY_COUNT,
} KeyboardButtons;

typedef enum {
    GAMEPAD_BTN_LEFT_UP,
    GAMEPAD_BTN_LEFT_DOWN,
    GAMEPAD_BTN_LEFT_LEFT,
    GAMEPAD_BTN_LEFT_RIGHT,
} GamepadButtons;

typedef struct {
    u32 btns;
    u32 btns_prev;
} Keyboard;

typedef struct {
    u32 btns;
} Gamepad;

typedef struct {
    f32 x;
    f32 y;
    u32 btns;
} Mouse;

typedef struct {
    Mouse mouse;
    Keyboard kb;
    Gamepad gamepad;
} Input;

#define IS_SET(mask, bit) ((mask) & (1U << (bit)))

void input_process(Input* input, SDL_Event* ev);
void input_clear(Input* input);
bool input_is_key_pressed(Input* input, KeyboardButtons btn);
bool input_is_key_down(Input* input, KeyboardButtons btn);
bool input_is_mouse_btn_pressed(Input* input, MouseButtons btn);
bool input_is_gamepad_btn_pressed(Input* input, GamepadButtons btn);

#endif // !INPUT_H_
