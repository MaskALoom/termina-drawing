#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SDL3/SDL.h>
#include "globals.hpp"
#include <vector>

enum class CameraState{
    STATE_ZOOMING = 0,
    STATE_NONE
};

class Camera{
private:
    int width = 0;
    int height = 0;

    float zoomBaseValue = 100.0f;

    std::vector<float> zoomValues;
    int zoomIndex = 8;
    int zoomIndexMax = 20;
public:
    CameraState state = CameraState::STATE_NONE;
    Vector2 pos = {0.0f, 0.0f};
    float zoom = 1.0f;

    bool isCameraMoving = false;

    void Init(void);
    void AlterZoom(void);
    void MoveCamera(Vector2 difference);
};

#endif



