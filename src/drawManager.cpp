#include "../include/drawManager.hpp"

#include <SDL3/SDL_mouse.h>
#include <iostream>

void DrawManager::CreateRect(float x, float y, float width, float height, Color color, Camera& camera){
    bool flipActive = false;
    if(camera.flipActive) flipActive = true;
    Rectangle rect = {{x, y}, {width, height}, color, flipActive};
    rects.push_back(rect);
}
void DrawManager::Update(Camera& camera){
    const bool* keyboard = SDL_GetKeyboardState(nullptr);
    if(!keyboard[SDL_SCANCODE_R]) return;

    Vector2 mousePos;
    SDL_GetMouseState(&mousePos.x, &mousePos.y);

    Vector2 canvasPos = {(mousePos.x - WINDOW_WIDTH / 2.0f) / camera.zoom + camera.pos.x, (mousePos.y - WINDOW_HEIGHT / 2.0f) / camera.zoom + camera.pos.y};

    Vector2 squareSize = {5, 5};
    Vector2 squarePos = canvasPos;
    squarePos.x += WINDOW_WIDTH / 2.0f;
    squarePos.y += WINDOW_HEIGHT / 2.0f;
    CreateRect(squarePos.x, squarePos.y, squareSize.x, squareSize.y, RED, camera);

    std::cout << "POS X: " << squarePos.x << " -- POS Y: " << squarePos.y << std::endl;
}
void DrawManager::RectZoomAdjust(Camera& camera){
    return;
}
void DrawManager::Draw(Camera& camera){
    for(auto rect : rects){
        Vector2 canvasCenterPos = {WINDOW_WIDTH / 2.0f / camera.zoom + camera.pos.x, WINDOW_HEIGHT / 2.0f / camera.zoom + camera.pos.y};

        if(camera.flipActive){
            if(!rect.flipActive){
                rect.flipActive = true;
                float xDifference = canvasCenterPos.x - rect.pos.x - rect.size.x / 2;

                rect.pos.x += xDifference*2;
            }
        }

        float newRectPosX = rect.pos.x - camera.pos.x - WINDOW_WIDTH / 2.0f + rect.size.x / 2.0f;
        float newRectPosY = rect.pos.y - camera.pos.y - WINDOW_HEIGHT / 2.0f + rect.size.y / 2.0f;

        float screenX = newRectPosX * camera.zoom + WINDOW_WIDTH / 2.0f;
        float screenY = newRectPosY * camera.zoom + WINDOW_HEIGHT / 2.0f;

        float newWidth = rect.size.x*camera.zoom;
        float newHeight = rect.size.y*camera.zoom;

        float newX = screenX - newWidth / 2.0f;
        float newY = screenY - newWidth / 2.0f;

        SDL_FRect finalRect = {newX, newY, newWidth, newHeight};
        SDL_SetRenderDrawColor(renderer, rect.color.a, rect.color.b, rect.color.g, rect.color.a);
        SDL_RenderFillRect(renderer, &finalRect);

        const bool* keyboard = SDL_GetKeyboardState(nullptr);
        if(!keyboard[SDL_SCANCODE_T]) return;

        std::cout << "pos X: " << canvasCenterPos.x << std::endl;
        //std::cout << "pos X: " << newX << std::endl;
    }
}



