#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <vector>

#include "globals.hpp"
#include "mouse.hpp"
#include <array>

typedef struct{
    Vector2 pos;
    float size;
    float penPressure;
}StrokePointData;

typedef struct{
    std::vector<StrokePointData> strokeData;
    Color color;
}Stroke;

class StrokeHandler;

class Canvas{
private:
    std::vector<Stroke> strokes;
    std::vector<Stroke> removedStrokes;

    float canvasWidth;
    float canvasHeight;
    float canvasPosX;
    float canvasPosY;
    Color backgroundColor;

    int currentDrawPointCount = 0;
public:
    SDL_Renderer* renderer;
    Camera* camera;
    StrokeHandler* strokeHandler;

    void CanvasInit(float width, float height, Color color);
    void AddStroke(const Stroke& stroke);
    void RemoveLastStroke(void);
    void RegainLastRemovedStroke(void);
    void ChangeBackgroundColor(Color color);
    void DrawCanvasBackground(void);
    void DrawCanvasGrid(void);

    void DrawCanvasAndStrokes(void);

    void CanvasReset(void);
    
    //Functions for postion conversion
    StrokePointData ConvertSPosToCPos(StrokePointData& pointData);
    void DrawPoint(StrokePointData& pointData, Color pointColor);
    void DrawLine(Vector2 posStart, Vector2 posEnd, Color color);
    void ConvertPosRelToCamera(Vector2* pos, Vector2* size);
    void DrawRect(Vector2 pos, Vector2 size, Color color);
};

class StrokeHandler{
private:
    float brushSize;
    float brushMin;
    float opacityMin;
    Canvas* canvasP;

    Stroke activeStroke;

    std::vector<PathPoint> pathProcessingBuffer;

    std::array<float, 50> brushSizes;
    int brushMaxSize = 50 - 1;
    int brushIndex;
public:
    Color tempColor = GetMonochromeColor(75);

    void Init(float initBrushSize, float initBrushMin, float initOpacityMin, Canvas* canvas);
    Stroke GetActiveStroke(void);
    void MousePathStrokeInterpolation(Mouse& mouse);
    void ProcessBuffer(void);
    void CheckValidDistance(void);
    void IncreaseBrushSize(void);
    void DecreaseBrushSize(void);
};

class CanvasInputHandler{
private:
    void Undo(void);
    void Redo(void);
    void ChangeBrushSize(void);
public:
    Canvas* canvas;
    StrokeHandler* strokeHandler;

    void Init(Canvas* canvasP, StrokeHandler* strokeHandlerP);
    void Update(void);
};

#endif









