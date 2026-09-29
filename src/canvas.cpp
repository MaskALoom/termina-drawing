#include "../include/canvas.hpp"

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
void Canvas::DrawCanvasBackground(void){
    Vector2 pos = {canvasPosX, canvasPosY};
    Vector2 size = {canvasWidth, canvasHeight};
    DrawConvertPosRelToCamera(pos, size, backgroundColor);
    DrawConvertPosRelToCamera({100, 100}, {200, 200}, RED);
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
            ProcessBuffer();
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
void Canvas::DrawConvertPosRelToCamera(Vector2 pos, Vector2 size, Color color){
    if(pos.x > canvasWidth || pos.x < 0) return;
    if(pos.y > canvasHeight || pos.y < 0) return;

    float newRectPosX = pos.x - camera->pos.x - WINDOW_WIDTH / 2.0f + size.x / 2.0f;
    float newRectPosY = pos.y - camera->pos.y - WINDOW_HEIGHT / 2.0f + size.y / 2.0f;

    float screenX = newRectPosX * camera->zoom + WINDOW_WIDTH / 2.0f;
    float screenY = newRectPosY * camera->zoom + WINDOW_HEIGHT / 2.0f;

    float newWidth = size.x * camera->zoom;
    float newHeight = size.y * camera->zoom;

    float newX = screenX - newWidth / 2.0f;
    float newY = screenY - newHeight / 2.0f;

    SDL_FRect finalRect = {newX, newY, newWidth, newHeight};

    if(color.a < 255){
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &finalRect);
}
void Canvas::DrawPoint(StrokePointData& pointData, Color pointColor){
    DrawConvertPosRelToCamera({pointData.pos.x, pointData.pos.y}, {pointData.size, pointData.size}, pointColor);
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









