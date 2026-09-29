#ifndef MOUSE_HPP
#define MOUSE_HPP

#include <SDL3/SDL.h>
#include "globals.hpp"
#include <iostream>
#include "camera.hpp"

#include <vector>

enum class InputDevice{
    NONE = 0,
    PEN_TABLET,
    MOUSE
};

typedef struct{
    Vector2 pos;
    float pressure;
}PathPoint;

class Mouse{
private:
    Vector2 lastPos = {0.0f, 0.0f};
    Vector2 pos = {0.0f, 0.0f};
    float pressureValue = 1.0f;

    InputDevice activeDevice = InputDevice::NONE;
public:
    std::vector<PathPoint> mousePathBuffer;
    bool isDrawing = false;
    bool pressureValid = false;

    void MouseUpdate(Camera& camera);
    void GetMousePath(SDL_Event& event);
    void CheckActiveInputDevice(void);
    void PenPressureReset(void);
    Vector2 GetMouseDifference(void);
    Vector2 GetMousePosition(void);
};

#endif



