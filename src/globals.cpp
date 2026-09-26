#include "../include/globals.hpp"

SDL_Keycode globalKeyPressed = SDLK_UNKNOWN;

bool IsKeyPressed(SDL_Keycode key){
    if(key == globalKeyPressed) return true;
    else return false;
}
bool IsKeyHeld(SDL_Scancode key){
    const bool* keyboard = SDL_GetKeyboardState(nullptr);

    if(keyboard[key]) return true;
    else return false;
}
