#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#define RED (Color){255, 0, 0, 255}
#define GREY (Color){200, 200, 200, 255}

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 650

#include <SDL3/SDL.h>

extern SDL_Keycode globalKeyPressed;

typedef struct{
    int r;
    int g;
    int b;
    int a;
}Color;

typedef struct{
    float x;
    float y;
}Vector2;

bool IsKeyPressed(SDL_Keycode key);
bool IsKeyHeld(SDL_Scancode key);

#endif
