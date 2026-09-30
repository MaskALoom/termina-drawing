#include "../include/canvas.hpp"
#include <SDL3/SDL_render.h>

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
    int index = 0;
    for(int i = 0; i < canvasWidth; ++i){
        Vector2 posStart = {(float)i, 0};
        Vector2 posEnd = {(float)i, canvasHeight};
        DrawLine(posStart, posEnd, BLACK);
        if(index % 5 == 0){
            Vector2 posStart = {(float)i-0.2f, 0};
            Vector2 posEnd = {(float)i-0.2f, canvasHeight};
            for(int j = 0; j < 5; ++j){
                DrawLine(posStart, posEnd, BLACK);

                posStart.x += 0.1f;
                posEnd.x += 0.1f;
            }
        }
        ++index;
    }
    index = 0;
    for(float i = 0; i < canvasHeight; ++i){
        Vector2 posStart = {0, i};
        Vector2 posEnd = {canvasWidth, i};
        DrawLine(posStart, posEnd, BLACK);
        if(index % 5 == 0){
            Vector2 posStart = {0, i-0.3f};
            Vector2 posEnd = {canvasWidth, i-0.3f};
            for(int j = 0; j < 5; ++j){
                DrawLine(posStart, posEnd, BLACK);

                posStart.y += 0.1f;
                posEnd.y += 0.1f;
            }
        }
        ++index;
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
    }
    brushIndex = 8;
    brushSize = brushSizes[brushIndex];

    activeStroke.color = tempColor;
}
Stroke StrokeHandler::GetActiveStroke(void){
    return activeStroke;
}

//Code AI Written / Clean up / Unfuck this code from a previous written fuction
void StrokeHandler::ProcessBuffer(void)
{
    std::vector<PathPoint> newPathPoints;

    for (size_t index = 0; index + 1 < pathProcessingBuffer.size(); ++index){
        PathPoint& pointStart = pathProcessingBuffer[index];
        PathPoint& pointEnd   = pathProcessingBuffer[index + 1];

        float xDifference = pointEnd.pos.x - pointStart.pos.x;
        float yDifference = pointEnd.pos.y - pointStart.pos.y;

        float biggerDifference = std::max(std::abs(xDifference), std::abs(yDifference));

        /*
        StrokePointData tempStartPos = {0.0f, 0.0f, 0.0f, 0.0f};
        StrokePointData tempEndPos = {0.0f, 0.0f, 0.0f, 0.0f};

        tempStartPos.pos = {pointStart.pos.x, pointStart.pos.y};
        tempEndPos.pos = {pointEnd.pos.x, pointEnd.pos.y};

        tempStartPos = canvasP->ConvertSPosToCPos(tempStartPos);
        tempEndPos = canvasP->ConvertSPosToCPos(tempEndPos);

        float xDifference = tempEndPos.pos.x - tempStartPos.pos.x;
        float yDifference = tempEndPos.pos.y - tempStartPos.pos.y;

        float biggerDifference = std::max(std::abs(xDifference), std::abs(yDifference));

        std::cout << "difference: " << biggerDifference << std::endl;
        */

        if (biggerDifference <= 1.0f){
            newPathPoints.push_back(pointStart);
            continue;
        }


        float xIncrement = xDifference / biggerDifference;
        float yIncrement = yDifference / biggerDifference;

        float pressureDifference = pointEnd.pressure - pointStart.pressure;

        float pressureIncrement = pressureDifference / biggerDifference;

        for (int i = 0; i < biggerDifference; ++i){
            float newX = pointStart.pos.x + xIncrement * i;
            float newY = pointStart.pos.y + yIncrement * i;
            float newPressure = pointStart.pressure + pressureIncrement * i;

            newPathPoints.push_back({newX, newY, newPressure});
        }
    }
    if(!pathProcessingBuffer.empty()) newPathPoints.push_back(pathProcessingBuffer.back());

    for (auto& mousePos : newPathPoints) {
        float sizeDifference = brushSize - brushMin;
        float newSize = brushMin + sizeDifference * mousePos.pressure;

        StrokePointData newData = {{mousePos.pos.x, mousePos.pos.y}, newSize, mousePos.pressure};
        StrokePointData newPoint = canvasP->ConvertSPosToCPos(newData);
        activeStroke.strokeData.push_back(newPoint);
    }
    PathPoint lastBufferPoint = pathProcessingBuffer.back();
    pathProcessingBuffer.clear();
    pathProcessingBuffer.push_back(lastBufferPoint);
}
void StrokeHandler::CheckValidDistance(Mouse& mouse){
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

        float xDifference = tempEndPos.pos.x - tempStartPos.pos.x;
        float yDifference = tempEndPos.pos.y - tempStartPos.pos.y;

        float biggerDifference = std::max(std::abs(xDifference), std::abs(yDifference));

        if(biggerDifference <= 1.0f){
            continue;
        }
        else{
            newProcessingBuffer.push_back(point);
        }
    }
    pathProcessingBuffer = newProcessingBuffer;
}
void StrokeHandler::MousePathStrokeInterpolation(Mouse& mouse){
    if(!mouse.isDrawing){
        if(!activeStroke.strokeData.empty()){
            canvasP->AddStroke(activeStroke);
            activeStroke.strokeData.clear();
            pathProcessingBuffer.clear();
        }
        return;
    }
    std::vector<PathPoint> newPathPoints;
    if(mouse.mousePathBuffer.size() > 0){
        for(auto& pathPoint : mouse.mousePathBuffer){
            pathProcessingBuffer.push_back(pathPoint);
        }
        mouse.mousePathBuffer.clear();
        if(pathProcessingBuffer.size() > 1){
            //CheckValidDistance(mouse);
            if(pathProcessingBuffer.size() > 1){
                ProcessBuffer();
                //Meant for some weird point overlap
                activeStroke.strokeData.pop_back();
            }
        }
    }
}
void StrokeHandler::IncreaseBrushSize(void){
    ++brushIndex;
    if(brushIndex > brushMaxSize) brushIndex = brushMaxSize;
    brushSize = brushSizes[brushIndex];
}
void StrokeHandler::DecreaseBrushSize(void){
    --brushIndex;
    if(brushIndex < 0) brushIndex = 0;
    brushSize = brushSizes[brushIndex];
}
void Canvas::CanvasReset(void){
    strokes.clear();
}

//Work with this shit
StrokePointData Canvas::ConvertSPosToCPos(StrokePointData& pointData){
    float canvasPosX = (pointData.pos.x - WINDOW_WIDTH / 2.0f) / camera->zoom + camera->pos.x;
    float canvasPosY = (pointData.pos.y - WINDOW_HEIGHT / 2.0f) / camera->zoom + camera->pos.y;

    Vector2 canvasPos = {canvasPosX, canvasPosY};

    Vector2 squarePos = canvasPos;

    //Drawn square screen offset
    squarePos.x += WINDOW_WIDTH / 2.0f - pointData.size / 2.0f;
    squarePos.y += WINDOW_HEIGHT / 2.0f - pointData.size / 2.0f;

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









