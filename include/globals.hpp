#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#define RED (Color){255, 0, 0, 255}
#define FADE_RED (Color){255, 0, 0, 100}
#define GREY (Color){200, 200, 200, 255}
#define BLACK (Color){0, 0, 0, 255}
#define WHITE (Color){255, 255, 255, 255}

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 650

#include <SDL3/SDL.h>

extern SDL_Keycode globalKeyPressed;
extern Uint8 globalButtonPressed;
extern Uint8 globalButtonReleased;
extern bool globalPenHeld;
extern bool globalPenReleased;

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
//bool IsKeyReleased()
bool IsButtonClicked(Uint8 button);
bool IsButtonReleased(Uint8 button);
bool IsButtonHeld(SDL_MouseButtonFlags button);

//bool IsPenButtonClicked(void);
bool IsPenButtonHeld(void);
bool IsPenButtonReleased(void);

Color Fade(Color color, float fade);

void ResetGlobalKeysAndButtons(void);
Color GetMonochromeColor(char value);

#endif










