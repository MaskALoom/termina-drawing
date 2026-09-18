#include "../include/mouse.hpp"

void Mouse::MouseUpdate(void){
    lastPos = pos;
    SDL_GetMouseState(&pos.x, &pos.y);
}
Vector2 Mouse::GetMouseDifference(void){
    const bool* keyboard = SDL_GetKeyboardState(nullptr);
    if(keyboard[SDL_SCANCODE_SPACE]){
        //if(!keyboard[SDL_SCANCODE_E]) return;
        float xDifference = pos.x - lastPos.x;
        float yDifference = pos.y - lastPos.y;
        
        return {xDifference, yDifference};
    }
    return {0.0f, 0.0f};
}
