#include "../include/camera.hpp"
#include <iostream>

void Camera::Init(void){
    float baseValue = 1.0f;
    for(int i = 0; i < 100; ++i){
        zoomValues.push_back(baseValue);
        baseValue += baseValue * 0.8f;
    }
}

void Camera::AlterZoom(void){
    if(IsKeyPressed(SDLK_Q)){
        --zoomIndex;
        if(zoomIndex < 0) zoomIndex = 0;
        zoomBaseValue = zoomValues[zoomIndex];
    }
    else if(IsKeyPressed(SDLK_W)){
        ++zoomIndex;
        if(zoomIndex > zoomIndexMax) zoomIndex = zoomIndexMax;
        zoomBaseValue = zoomValues[zoomIndex];
    }
    /*
    if(IsKeyPressed(SDLK_Q)){
        zoomBaseValue -= 10.0f;
        std::cout << "ZOOM: " << zoomBaseValue << std::endl;
    }
    else if(IsKeyPressed(SDLK_W)){
        zoomBaseValue += 10.0f;
        std::cout << "ZOOM: " << zoomBaseValue << std::endl;
    }
    if(zoomBaseValue <= 0.0f) zoom = 1.0f;
    else if(zoomBaseValue >= 6000.0f) zoom = 6000.0f;
    */

    zoom = zoomBaseValue / 100.0f;
}
void Camera::MoveCamera(Vector2 difference){
    if(!IsKeyHeld(SDL_SCANCODE_SPACE)) return;

    pos.x -= difference.x / zoom;
    pos.y -= difference.y / zoom;
}







