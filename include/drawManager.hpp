#ifndef DRAW_MANAGER_HPP
#define DRAW_MANAGER_HPP

#include <SDL3/SDL.h>

#include "globals.hpp"
#include "camera.hpp"
#include "mouse.hpp"

#include <vector>

typedef struct{
    Vector2 pos;
    Vector2 size;
    Color color;
}DrawRectData;

class Stroke{
private:
public:
    std::vector<DrawRectData> strokeData;
    //void DrawStroke(void);
};

class DrawManager{
private:
    std::vector<DrawRectData> rects;
    std::vector<Stroke> strokes;
    Stroke activeStroke;

    float brushSize = 7;
public:
    SDL_Renderer* renderer;
    Camera* camera;

    float canvasWidth;
    float canvasHeight;

    bool drawingActive = false;

    void Init(float width, float height, Color color);
    Vector2 ConvertPosition(float posX, float posY);
    void CreateRect(float x, float y, float width, float height, Color color);
    void Update(Mouse& mouse);
    void Draw(void);
    void DrawStroke(Stroke& stroke);
    void DrawRect(DrawRectData& rect);

    void Undo(Mouse& mouse);
};

Color GetMonochromeColor(char value);

#endif














