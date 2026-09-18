#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SDL3/SDL.h>
#include "globals.hpp"

enum class CameraState{
    STATE_ZOOMING = 0,
    STATE_NONE
};

class Camera{
private:
public:
    CameraState state = CameraState::STATE_NONE;

    Vector2 pos = {0.0f, 0.0f};
    int width = 0;
    int height = 0;

    float zoom = 1.0f;
    bool flipActive = false;

    void AlterZoom(void);
    void CameraFlip(void);
   void MoveCamera(Vector2 difference);
};

#endif



