#include "../include/camera.hpp"
#include <iostream>

void Camera::AlterZoom(void){
    /*
    if(IsKeyPressed(SDLK_Q)){
        zoom -= 0.1f;
    }
    else if(IsKeyPressed(SDLK_W)){
        zoom += 0.1f;
    }
    */
    const bool* keyboard = SDL_GetKeyboardState(nullptr);
    if(keyboard[SDL_SCANCODE_Q]){
        if(zoom == 0.5f) return;
        state = CameraState::STATE_ZOOMING;
        zoom = 0.5f;
    }
    else if(keyboard[SDL_SCANCODE_W]){
        if(zoom == 1.5f) return;
        state = CameraState::STATE_ZOOMING;
        zoom = 1.5f;
    }

    if(zoom <= 0.0f) zoom = 0.05f;
    else if(zoom >= 15.0f) zoom = 15.0f;
}
void Camera::MoveCamera(Vector2 difference){
    if(!IsKeyHeld(SDL_SCANCODE_SPACE)) return;

    pos.x -= difference.x / zoom;
    pos.y -= difference.y / zoom;
}







