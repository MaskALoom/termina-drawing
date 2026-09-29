#include "../include/mouse.hpp"

void Mouse::CheckActiveInputDevice(void){
    if(IsPenButtonReleased() && activeDevice == InputDevice::PEN_TABLET){
        activeDevice = InputDevice::NONE;
    }
    if(IsButtonReleased(SDL_BUTTON_LEFT) && activeDevice == InputDevice::MOUSE){
        activeDevice = InputDevice::NONE;
    }

    //Check which device button is being held down
    if(IsPenButtonHeld() && activeDevice == InputDevice::NONE){
        activeDevice = InputDevice::PEN_TABLET;
    }
    else if(IsButtonHeld(SDL_BUTTON_LEFT) && activeDevice != InputDevice::PEN_TABLET){
        activeDevice = InputDevice::MOUSE;
    }
}
void Mouse::GetMousePath(SDL_Event& event){
    if(!isDrawing){
        pressureValue = false;
        return;
    }
    if(event.type == SDL_EVENT_PEN_AXIS){
        if(event.paxis.axis == SDL_PEN_AXIS_PRESSURE){
            pressureValue = event.paxis.value;
            pressureValid = true;
        }
    }
    else if(event.type == SDL_EVENT_PEN_MOTION && activeDevice == InputDevice::PEN_TABLET){
        if(!pressureValid) return;
        if(isDrawing) mousePathBuffer.push_back({event.pmotion.x, event.pmotion.y, pressureValue});
        //std::cout << "triggered pen motion" << std::endl;
        return;
    }
    else if(event.type == SDL_EVENT_MOUSE_MOTION && activeDevice == InputDevice::MOUSE){
        if(isDrawing) mousePathBuffer.push_back({event.motion.x, event.motion.y, 1.0f});
        //std::cout << "triggered mouse motion" << std::endl;
    }
}
void Mouse::PenPressureReset(void){
    pressureValue = 1.0f;
}
void Mouse::MouseUpdate(Camera& camera){
    lastPos = pos;
    SDL_GetMouseState(&pos.x, &pos.y);

    if(camera.isCameraMoving) return;

    if(!IsButtonHeld(SDL_BUTTON_LMASK)){
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
