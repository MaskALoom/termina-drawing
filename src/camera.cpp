#include "../include/camera.hpp"
#include <iostream>

void Camera::Init(void){
    float baseValue = 1.0f;
    for(int i = 0; i < 20; ++i){
        zoomValues.push_back(baseValue);
        baseValue += baseValue * 0.8f;
    }
}

void Camera::AlterZoom(void){
    //if(!IsKeyPressed(SDLK_Q) || !IsKeyPressed(SDLK_W)) return;
    if(IsKeyPressed(SDLK_Q)){
        --zoomIndex;
        if(zoomIndex < 0) zoomIndex = 0;
        zoomBaseValue = zoomValues[zoomIndex];
    }
    else if(IsKeyPressed(SDLK_W)){
        ++zoomIndex;
        if(zoomIndex > zoomIndexMax - 1) zoomIndex = zoomIndexMax - 1;
        zoomBaseValue = zoomValues[zoomIndex];
    }
    zoom = zoomBaseValue / 100.0f;

}
void Camera::MoveCamera(Vector2 difference){
    if(!IsKeyHeld(SDL_SCANCODE_SPACE)){
        isCameraMoving = false;
        return;
    }
    else isCameraMoving = true;
    if(!IsButtonHeld(SDL_BUTTON_LMASK)) return;

    pos.x -= difference.x / zoom;
    pos.y -= difference.y / zoom;
}







