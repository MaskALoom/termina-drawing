#include "../include/globals.hpp"
#include <SDL3/SDL_mouse.h>

SDL_Keycode globalKeyPressed = SDLK_UNKNOWN;
Uint8 globalButtonPressed = 0;
Uint8 globalButtonReleased = 0;
bool globalPenHeld = false;
bool globalPenReleased = false;

Color GetMonochromeColor(int value){
    return {value, value, value, 255};
}

Color Fade(Color color, float fade){
    Color newColor = color;
    newColor.a = newColor.a * fade;
    return newColor;
}

void ResetGlobalKeysAndButtons(void){
    globalKeyPressed = SDLK_UNKNOWN;
    globalButtonPressed = 0;
    globalButtonReleased = 0;
    globalPenHeld = false;
    globalPenReleased = false;
}

bool IsKeyPressed(SDL_Keycode key){
    if(key == globalKeyPressed) return true;
    else return false;
}
bool IsKeyHeld(SDL_Scancode key){
    const bool* keyboard = SDL_GetKeyboardState(nullptr);

    if(keyboard[key]) return true;
    else return false;
}
bool IsButtonClicked(Uint8 button){
    if(globalButtonPressed == button){
        globalButtonPressed = 0;
        return true;
    }
    return false;
}
bool IsButtonHeld(SDL_MouseButtonFlags button){
    float x, y;
    SDL_MouseButtonFlags buttons = SDL_GetMouseState(&x, &y);
    if(buttons && button){
        return true;
    }
    return false;
}
bool IsButtonReleased(Uint8 button){
    if(globalButtonReleased == button){
        return true;
    }
    return false;
}

bool IsPenButtonHeld(void){
    if(globalPenHeld){
        return true;
    }
    else return false;
}
bool IsPenButtonReleased(void){
    if(globalPenReleased){
        return true;
    }
    else return false;
}









