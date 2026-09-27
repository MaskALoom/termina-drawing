#include "../include/mouse.hpp"

void Mouse::MouseUpdate(void){
    lastPos = pos;
    SDL_GetMouseState(&pos.x, &pos.y);

    if(!IsKeyHeld(SDL_SCANCODE_R)){
        isDrawing = false;
        return;
    }
    isDrawing = true;
}
Vector2 Mouse::GetMouseDifference(void){
    float xDifference = pos.x - lastPos.x;
    float yDifference = pos.y - lastPos.y;
    
    return {xDifference, yDifference};
}
Vector2 Mouse::GetMousePosition(void){
    return {pos.x, pos.y};
}
