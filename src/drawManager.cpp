#include "../include/drawManager.hpp"
#include "../include/mouse.hpp"

#include <SDL3/SDL_mouse.h>
#include <iostream>

Color GetMonochromeColor(char value){
    return {value, value, value, value};
}

void DrawManager::Init(float width, float height, Color color){
    canvasWidth = width;
    canvasHeight = height;
    float posX = 0;
    float posY = 350 - 100;

    CreateRect(posX, posY, width, height, color);
}

void DrawManager::CreateRect(float x, float y, float width, float height, Color color){
    Rectangle rect = {{x, y}, {width, height}, color};
    rects.push_back(rect);
}
void DrawManager::Update(Camera& camera, Mouse& mouse){
    if(!IsKeyHeld(SDL_SCANCODE_R)) return;

    //Converstion from screen postion to canvas space relative to camera postion and zoom
    float canvasPosX = (mouse.GetMousePosition().x - WINDOW_WIDTH / 2.0f) / camera.zoom + camera.pos.x;
    float canvasPosY = (mouse.GetMousePosition().y - WINDOW_HEIGHT / 2.0f) / camera.zoom + camera.pos.y;

    Vector2 canvasPos = {canvasPosX, canvasPosY};

    Vector2 squareSize = {10, 10};
    Vector2 squarePos = canvasPos;

    //Drawn square screen offset
    //
    squarePos.x += WINDOW_WIDTH / 2.0f - squareSize.x / 2.0f;
    squarePos.y += WINDOW_HEIGHT / 2.0f - squareSize.y / 2.0f;

    CreateRect(squarePos.x, squarePos.y, squareSize.x, squareSize.y, RED);

    std::cout << "POS X: " << squarePos.x << " -- POS Y: " << squarePos.y << std::endl;
    std::cout << "Size: " << rects.size() << std::endl;
}
void DrawManager::Draw(Camera& camera){
    for(auto rect : rects){
        if(rect.pos.x > canvasWidth || rect.pos.x < 0) continue;
        if(rect.pos.y > canvasHeight || rect.pos.y < 0) continue;

        float newRectPosX = rect.pos.x - camera.pos.x - WINDOW_WIDTH / 2.0f + rect.size.x / 2.0f;
        float newRectPosY = rect.pos.y - camera.pos.y - WINDOW_HEIGHT / 2.0f + rect.size.y / 2.0f;

        float screenX = newRectPosX * camera.zoom + WINDOW_WIDTH / 2.0f;
        float screenY = newRectPosY * camera.zoom + WINDOW_HEIGHT / 2.0f;

        float newWidth = rect.size.x * camera.zoom;
        float newHeight = rect.size.y * camera.zoom;

        float newX = screenX - newWidth / 2.0f;
        float newY = screenY - newWidth / 2.0f;

        SDL_FRect finalRect = {newX, newY, newWidth, newHeight};
        SDL_SetRenderDrawColor(renderer, rect.color.r, rect.color.g, rect.color.b, rect.color.a);
        SDL_RenderFillRect(renderer, &finalRect);
    }
}



