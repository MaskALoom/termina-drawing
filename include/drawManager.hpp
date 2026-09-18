#ifndef DRAW_MANAGER_HPP
#define DRAW_MANAGER_HPP

#include <SDL3/SDL.h>

#include <vector>

#include "globals.hpp"
#include "camera.hpp"

typedef struct{
    Vector2 pos;
    Vector2 size;
    Color color;
    bool flipActive;
}Rectangle;

class DrawManager{
private:
    std::vector<Rectangle> rects;
public:
    SDL_Renderer* renderer;

    void CreateRect(float x, float y, float width, float height, Color color, Camera& camera);
    void Update(Camera& camera);
    void RectZoomAdjust(Camera& camera);
    void Draw(Camera& camera);
};

#endif














