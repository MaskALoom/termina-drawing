#include "../include/canvas.hpp"
#include <SDL3/SDL_render.h>
#include <cmath>

void Canvas::CanvasInit(float width, float height, Color color){
    canvasWidth = width;
    canvasHeight = height;
    canvasPosX = 0.0f;
    canvasPosY = 0.0f;
    backgroundColor = color;
}
void Canvas::AddStroke(const Stroke& stroke){
    strokes.push_back(stroke);
    currentDrawPointCount += stroke.strokeData.size();
    std::cout << "Draw Point Count: " << currentDrawPointCount << std::endl;
}
void Canvas::RemoveLastStroke(void){
    if(strokes.empty()) return;

    removedStrokes.push_back(strokes[strokes.size()-1]);
    strokes.erase(strokes.begin() + strokes.size() - 1);
}
void Canvas::RegainLastRemovedStroke(void){
    if(removedStrokes.empty()) return;

    strokes.push_back(removedStrokes[removedStrokes.size()-1]);
    removedStrokes.erase(removedStrokes.begin() + removedStrokes.size() - 1);
}
void Canvas::ChangeBackgroundColor(Color color){
    backgroundColor = color;
}
void Canvas::DrawCanvasGrid(void){
    Color lineColor = GetMonochromeColor(50);
    for(int i = 0; i < canvasWidth; ++i){
        Vector2 posStart = {(float)i, 0};
        Vector2 posEnd = {(float)i, canvasHeight};

        if(i % 10 == 0) lineColor = BLACK;
        else if(i % 5 == 0) lineColor = GetMonochromeColor(100);
        else lineColor = GetMonochromeColor(150);

        DrawLine(posStart, posEnd, lineColor);
    }
    for(int i = 0; i < canvasHeight; ++i){
        Vector2 posStart = {0, (float)i};
        Vector2 posEnd = {canvasWidth, (float)i};

        if(i % 10 == 0) lineColor = BLACK;
        else if(i % 5 == 0) lineColor = GetMonochromeColor(100);
        else lineColor = GetMonochromeColor(150);

        DrawLine(posStart, posEnd, lineColor);
    }
}
void Canvas::DrawCanvasBackground(void){
    Vector2 pos = {canvasPosX, canvasPosY};
    Vector2 size = {canvasWidth, canvasHeight};
    DrawRect(pos, size, backgroundColor);
    DrawRect({100, 100}, {200, 200}, RED);
}
void Canvas::DrawCanvasAndStrokes(void){
    DrawCanvasBackground();
    for(auto& stroke : strokes){
        for(auto& point : stroke.strokeData){
            DrawPoint(point, stroke.color);
        }
    }
    for(auto& point : strokeHandler->GetActiveStroke().strokeData){
        DrawPoint(point, strokeHandler->tempColor);
    }
    if(camera->zoom > 18){
        DrawCanvasGrid();
    }
}
void StrokeHandler::Init(float initBrushSize, float initBrushMin, float initOpacityMin, Canvas* canvas){
    canvasP = canvas;
    brushSize = initBrushSize;
    brushMin = initBrushMin;
    opacityMin = initOpacityMin;

    float brushSizeInit = 1;
    float brushMult = 1.3;

    for(int i = 0; i < brushMaxSize + 1; ++i){
        brushSizes[i] = brushSizeInit;
        brushSizeInit *= brushMult;
        brushSizeInit = std::ceil(brushSizeInit);
    }
    activeStroke.color = tempColor;

    brushIndex = 0;
    brushSize = brushSizes[brushIndex];
}
Stroke StrokeHandler::GetActiveStroke(void){
    return activeStroke;
}

//Code AI Written / Clean up / Unfuck this code from a previous written fuction
void StrokeHandler::ProcessBuffer(void){
    std::vector<PathPoint> newPathPoints;
    for (size_t index = 0; index + 1 < pathProcessingBuffer.size(); ++index){
        PathPoint pointStart = pathProcessingBuffer[index];
        PathPoint pointEnd   = pathProcessingBuffer[index + 1];

        StrokePointData tempStartPos = {0.0f, 0.0f, 0.0f, 0.0f};
        StrokePointData tempEndPos = {0.0f, 0.0f, 0.0f, 0.0f};

        tempStartPos.pos = {pointStart.pos.x, pointStart.pos.y};
        tempEndPos.pos = {pointEnd.pos.x, pointEnd.pos.y};

        tempStartPos = canvasP->ConvertSPosToCPos(tempStartPos);
        tempEndPos = canvasP->ConvertSPosToCPos(tempEndPos);

        float xDifference = tempEndPos.pos.x - tempStartPos.pos.x;
        float yDifference = tempEndPos.pos.y - tempStartPos.pos.y;

        float biggerDifference = std::max(std::abs(xDifference), std::abs(yDifference));

        xDifference = pointEnd.pos.x - pointStart.pos.x;
        yDifference = pointEnd.pos.y - pointStart.pos.y;

        std::cout << "Difference: " << biggerDifference << std::endl;

        if(biggerDifference <= 1.0f){
            if(!activeStroke.strokeData.empty() && index == 0) continue;

            newPathPoints.push_back(pointStart);
            continue;
        }
        std::cout << "Interpolation TRIGGERED!" << std::endl;
        std::cout << "Start X: " << pointStart.pos.x << " Start Y: " << pointStart.pos.y << std::endl;
        std::cout << "End X: " << pointEnd.pos.x << " End Y: " << pointEnd.pos.y << std::endl;

        //newPathPoints.push_back(pointStart);
        //return;

        float xIncrement = xDifference / biggerDifference;
        float yIncrement = yDifference / biggerDifference;

        float pressureDifference = pointEnd.pressure - pointStart.pressure;
        float pressureIncrement = pressureDifference / biggerDifference;

        for (int i = 0; i < biggerDifference; ++i){
            if(!activeStroke.strokeData.empty() && i == 0) continue;

            float newX = pointStart.pos.x + xIncrement * i;
            float newY = pointStart.pos.y + yIncrement * i;
            float newPressure = pointStart.pressure + pressureIncrement * i;

            newPathPoints.push_back({newX, newY, newPressure});
        }
    }
    if(!pathProcessingBuffer.empty()) newPathPoints.push_back(pathProcessingBuffer.back());

    for (auto& mousePos : newPathPoints) {
        float sizeDifference = brushSize - brushMin;
        float newSize = std::round(brushMin + sizeDifference * mousePos.pressure);

        StrokePointData newData = {{mousePos.pos.x, mousePos.pos.y}, newSize, mousePos.pressure};

        //Final conersion of position to be drawn
        StrokePointData newPoint = canvasP->ConvertSPosToCPos(newData);

        newPoint.pos.x = std::floor(newPoint.pos.x);
        newPoint.pos.y = std::floor(newPoint.pos.y);

        activeStroke.strokeData.push_back(newPoint);
    }
    //Needed to know where the last point ended to continue the stroke order
    PathPoint lastBufferPoint = newPathPoints.back();
    pathProcessingBuffer.clear();
    pathProcessingBuffer.push_back(lastBufferPoint);
}
void StrokeHandler::CheckValidDistance(void){
    PathPoint pointStart = pathProcessingBuffer[0];
    std::vector<PathPoint> newProcessingBuffer;
    newProcessingBuffer.push_back(pointStart);
    for(auto& point : pathProcessingBuffer){
        StrokePointData tempStartPos = {0.0f, 0.0f, 0.0f, 0.0f};
        StrokePointData tempEndPos = {0.0f, 0.0f, 0.0f, 0.0f};

        tempStartPos.pos = {pointStart.pos.x, pointStart.pos.y};
        tempEndPos.pos = {point.pos.x, point.pos.y};

        tempStartPos = canvasP->ConvertSPosToCPos(tempStartPos);
        tempEndPos = canvasP->ConvertSPosToCPos(tempEndPos);

        Vector2 beforeRoundStart = tempStartPos.pos;
        Vector2 beforeRoundEnd = tempEndPos.pos;

        tempStartPos.pos.x = std::floor(tempStartPos.pos.x);
        tempStartPos.pos.y = std::floor(tempStartPos.pos.y);
        tempEndPos.pos.x = std::floor(tempEndPos.pos.x);
        tempEndPos.pos.y = std::floor(tempEndPos.pos.y);

        float xDifference = tempEndPos.pos.x - tempStartPos.pos.x;
        float yDifference = tempEndPos.pos.y - tempStartPos.pos.y;

        float biggerDifference = std::max(std::abs(xDifference), std::abs(yDifference));

        if(biggerDifference < 1.0f) continue;
        else{
            newProcessingBuffer.push_back(point);
            pointStart = point;
        }

        /*
        std::cout << "---------------" << std::endl;
        std::cout << "Start Pos X:" << tempStartPos.pos.x << " Start Pos Y: " << tempStartPos.pos.y << std::endl;
        std::cout << "End Pos X:" << tempEndPos.pos.x << " End Pos Y: " << tempEndPos.pos.y << std::endl;

        std::cout << "Start Pos X:" << beforeRoundStart.x << " Start Pos Y: " << beforeRoundStart.y << std::endl;
        std::cout << "End Pos X:" << beforeRoundEnd.x << " End Pos Y: " << beforeRoundEnd.y << std::endl;
        */
    }
    pathProcessingBuffer = newProcessingBuffer;
    if(pathProcessingBuffer.size() > 1){
        int index = 0;
        for(auto& pathPoint : pathProcessingBuffer){
            //std::cout << index << " = Start X: " << pathPoint.pos.x << " Start Y: " << pathPoint.pos.y << std::endl;
            ++index;
        }
    }
}
void StrokeHandler::MousePathStrokeInterpolation(Mouse& mouse){
    if(!mouse.isDrawing){
        pathProcessingBuffer.clear();
        if(!activeStroke.strokeData.empty()){
            std::cout << "before size: " << activeStroke.strokeData.size() << std::endl;
            std::cout << "------------------" << std::endl;
            std::cout << "------------------" << std::endl;
            std::cout << "------------------" << std::endl;
            if(activeStroke.strokeData.size() < 20){
                for(auto& point : activeStroke.strokeData){
                    std::cout << "point pos X: " << point.pos.x << " --- point pos Y: " << point.pos.y << std::endl;
                }
            }
            canvasP->AddStroke(activeStroke);
            activeStroke.strokeData.clear();
        }
        return;
    }
    std::vector<PathPoint> newPathPoints;
    if(mouse.mousePathBuffer.size() > 0){
        for(auto& pathPoint : mouse.mousePathBuffer){
            PathPoint newPathPoint = pathPoint;
            pathProcessingBuffer.push_back(pathPoint);
        }
        mouse.mousePathBuffer.clear();
        if(pathProcessingBuffer.size() > 1){
            CheckValidDistance();
            if(pathProcessingBuffer.size() > 1){
                ProcessBuffer();
                //Meant for some weird point overlap
                //activeStroke.strokeData.pop_back();
            }
        }
    }
}
void StrokeHandler::IncreaseBrushSize(void){
    ++brushIndex;
    if(brushIndex > brushMaxSize) brushIndex = brushMaxSize;
    brushSize = brushSizes[brushIndex];
    std::cout << "Brush Size: " << brushSize << std::endl;
}
void StrokeHandler::DecreaseBrushSize(void){
    --brushIndex;
    if(brushIndex < 0) brushIndex = 0;
    brushSize = brushSizes[brushIndex];
    std::cout << "Brush Size: " << brushSize << std::endl;
}
void Canvas::CanvasReset(void){
    strokes.clear();
}

//Position converting
StrokePointData Canvas::ConvertSPosToCPos(StrokePointData& pointData){
    float canvasPosX = (pointData.pos.x - WINDOW_WIDTH / 2.0f) / camera->zoom + camera->pos.x;
    float canvasPosY = (pointData.pos.y - WINDOW_HEIGHT / 2.0f) / camera->zoom + camera->pos.y;

    Vector2 canvasPos = {canvasPosX, canvasPosY};
    Vector2 squarePos = canvasPos;

    float newSize = pointData.size;
    if(newSize <= 1) newSize = 0;

    //Drawn square screen offset
    squarePos.x += WINDOW_WIDTH / 2.0f - newSize / 2.0f;
    squarePos.y += WINDOW_HEIGHT / 2.0f - newSize / 2.0f;

    return {squarePos.x, squarePos.y, pointData.size, pointData.penPressure};
}
void Canvas::ConvertPosRelToCamera(Vector2* pos, Vector2* size){
    if(pos->x > canvasWidth || pos->x < 0) return;
    if(pos->y > canvasHeight || pos->y < 0) return;

    float newRectPosX = pos->x - camera->pos.x - WINDOW_WIDTH / 2.0f + size->x / 2.0f;
    float newRectPosY = pos->y - camera->pos.y - WINDOW_HEIGHT / 2.0f + size->y / 2.0f;

    float screenX = newRectPosX * camera->zoom + WINDOW_WIDTH / 2.0f;
    float screenY = newRectPosY * camera->zoom + WINDOW_HEIGHT / 2.0f;

    float newWidth = size->x * camera->zoom;
    float newHeight = size->y * camera->zoom;

    pos->x = screenX - newWidth / 2.0f;
    pos->y = screenY - newHeight / 2.0f;

    size->x = newWidth;
    size->y = newHeight;
}

//Draw functions
void Canvas::DrawRect(Vector2 pos, Vector2 size, Color color){
    ConvertPosRelToCamera(&pos, &size);
    SDL_FRect finalRect = {pos.x, pos.y, size.x, size.y};

    if(color.a < 255){
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &finalRect);
}
void Canvas::DrawLine(Vector2 posStart, Vector2 posEnd, Color color){
    Vector2 size = {0, 0};
    ConvertPosRelToCamera(&posStart, &size);
    ConvertPosRelToCamera(&posEnd, &size);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_RenderLine(renderer, posStart.x, posStart.y, posEnd.x, posEnd.y);
}
void Canvas::DrawPoint(StrokePointData& pointData, Color pointColor){
    Vector2 newPos = pointData.pos;
    Vector2 newSize = {pointData.size, pointData.size};
    DrawRect(newPos, newSize, pointColor);
}

void CanvasInputHandler::Init(Canvas* canvasP, StrokeHandler* strokeHandlerP){
    canvas = canvasP;
    strokeHandler = strokeHandlerP;
}

void CanvasInputHandler::Undo(void){
    if(!IsKeyHeld(SDL_SCANCODE_LCTRL)) return;
    if(IsKeyPressed(SDLK_Z)) canvas->RemoveLastStroke();
}
void CanvasInputHandler::Redo(void){
    if(!IsKeyHeld(SDL_SCANCODE_LCTRL)) return;
    if(IsKeyPressed(SDLK_X)) canvas->RegainLastRemovedStroke();
}
void CanvasInputHandler::ChangeBrushSize(void){
    if(!IsKeyHeld(SDL_SCANCODE_LSHIFT)) return;
    if(IsKeyPressed(SDLK_S)){
        strokeHandler->IncreaseBrushSize();
    }
    else if(IsKeyPressed(SDLK_D)){
        strokeHandler->DecreaseBrushSize();
    }
}
void CanvasInputHandler::Update(void){
    Undo();
    Redo();
    ChangeBrushSize();
}









