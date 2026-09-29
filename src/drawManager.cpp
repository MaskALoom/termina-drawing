#include "../include/drawManager.hpp"
#include "../include/mouse.hpp"

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_render.h>
#include <iostream>

void DrawManager::BrushResize(void){
    if(IsKeyHeld(SDL_SCANCODE_LSHIFT)){
        if(IsKeyPressed(SDLK_S)){
            std::cout << "Brush Size: " << brushSize << std::endl;
            brushSize += brushSize * 0.4;
        }
        else if(IsKeyPressed(SDLK_D)){
            std::cout << "Brush Size: " << brushSize << std::endl;
            brushSize -= brushSize * 0.4;
        }
    }
    if(brushSize > 50) brushSize = 50;
    else if(brushSize < 1) brushSize = 1;
}

void DrawManager::Init(float width, float height, Color color){
    canvasWidth = width;
    canvasHeight = height;
    float posX = 0;
    float posY = 0;

    CreateRect(posX, posY, width, height, color);
}

void DrawManager::CreateRect(float x, float y, float width, float height, Color color){
    DrawRectData rect = {{x, y}, {width, height}, color};
    rects.push_back(rect);
}
Vector2 DrawManager::ConvertPosition(float posX, float posY){
    float canvasPosX = (posX - WINDOW_WIDTH / 2.0f) / camera->zoom + camera->pos.x;
    float canvasPosY = (posY - WINDOW_HEIGHT / 2.0f) / camera->zoom + camera->pos.y;

    Vector2 canvasPos = {canvasPosX, canvasPosY};

    Vector2 squarePos = canvasPos;

    //Drawn square screen offset
    squarePos.x += WINDOW_WIDTH / 2.0f - brushSize / 2.0f;
    squarePos.y += WINDOW_HEIGHT / 2.0f - brushSize / 2.0f;

    return {squarePos.x, squarePos.y};
}
void DrawManager::Update(Mouse& mouse){
    Undo(mouse);
    BrushResize();
    if(!mouse.isDrawing){
        if(!activeStroke.strokeData.empty()){
            strokes.push_back(activeStroke);
            activeStroke.strokeData.clear();
        }
        return;
    }
    for(auto& mousePos : mouse.mousePathBuffer){
        Vector2 newPos = ConvertPosition(mousePos.pos.x, mousePos.pos.y);
        activeStroke.strokeData.push_back({newPos, {brushSize, brushSize}, RED});
    }
    mouse.mousePathBuffer.clear();

    //std::cout << "Size: " << rects.size() << std::endl;
}
void DrawManager::DrawRect(DrawRectData& rect){
    if(rect.pos.x > canvasWidth || rect.pos.x < 0) return;
    if(rect.pos.y > canvasHeight || rect.pos.y < 0) return;

    float newRectPosX = rect.pos.x - camera->pos.x - WINDOW_WIDTH / 2.0f + rect.size.x / 2.0f;
    float newRectPosY = rect.pos.y - camera->pos.y - WINDOW_HEIGHT / 2.0f + rect.size.y / 2.0f;

    float screenX = newRectPosX * camera->zoom + WINDOW_WIDTH / 2.0f;
    float screenY = newRectPosY * camera->zoom + WINDOW_HEIGHT / 2.0f;

    float newWidth = rect.size.x * camera->zoom;
    float newHeight = rect.size.y * camera->zoom;

    float newX = screenX - newWidth / 2.0f;
    float newY = screenY - newHeight / 2.0f;

    SDL_FRect finalRect = {newX, newY, newWidth, newHeight};
    if(rect.color.a < 255){
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }
    SDL_SetRenderDrawColor(renderer, rect.color.r, rect.color.g, rect.color.b, rect.color.a);
    SDL_RenderFillRect(renderer, &finalRect);
}
void DrawManager::DrawStroke(Stroke& stroke){
    for(auto& rect : stroke.strokeData){
        DrawRect(rect);
    }
}
void DrawManager::Draw(void){
    for(auto& rect : rects){
        DrawRect(rect);
    }
    for(auto& stroke : strokes){
        DrawStroke(stroke);
    }
    DrawStroke(activeStroke);
}

void DrawManager::Undo(Mouse& mouse){
    if(mouse.isDrawing || strokes.empty()) return;
    if(IsKeyHeld(SDL_SCANCODE_LCTRL)){
        if(IsKeyPressed(SDLK_Z)){
            strokes.erase(strokes.begin() + strokes.size() - 1);
        }
    }
}







