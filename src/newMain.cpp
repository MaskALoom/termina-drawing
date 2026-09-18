#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>

#include <iostream>
#include <vector>

#include "../include/camera.hpp"
#include "../include/mouse.hpp"
#include "../include/drawManager.hpp"

SDL_Renderer* CreateRenderer(SDL_Window* window);
SDL_Window* CreateWindow(std::string windowName, int width, int height);

void DrawRectangle(float x, float y, float width, float height, Color color, SDL_Renderer* renderer, Camera& camera){
    //float newX = x-camera.x*camera.zoom;
    //float newY = y-camera.y*camera.zoom;

    float newX = x-camera.pos.x;
    float newY = y-camera.pos.y;
    float newWidth = width*camera.zoom;
    float newHeight = height*camera.zoom;

    SDL_FRect rect = {newX, newY, newWidth, newHeight};
    SDL_SetRenderDrawColor(renderer, color.a, color.b, color.g, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

int main(int argc, char** argv){
    Camera camera;
    Mouse mouse;
    DrawManager drawManager;
    SDL_Window* window = CreateWindow("Termina Drawing", WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_Renderer* renderer = CreateRenderer(window);
    //SDL_SetRenderVSync(renderer, 1);
    if(!window || !renderer){
        SDL_Log("SDL Error (Fix that shit): %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    drawManager.renderer = renderer;
    drawManager.CreateRect(0, 0, 200, 200, RED, camera);
    //drawManager.CreateRect(200, 200, 200, 200, RED);
    bool running = true;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
            if(event.type == SDL_EVENT_KEY_DOWN){
                if(event.key.key == SDLK_A){
                    camera.CameraFlip();
                }
            }
        }
        drawManager.Update(camera);
        mouse.MouseUpdate();
        camera.AlterZoom();
        drawManager.RectZoomAdjust(camera);
        camera.MoveCamera(mouse.GetMouseDifference());

        camera.state = CameraState::STATE_NONE;
        //Drawing
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);
        drawManager.Draw(camera);
        //DrawRectangle(400, 100, 200, 200, RED, renderer, camera);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}

SDL_Renderer* CreateRenderer(SDL_Window* window){
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    return renderer;
}

SDL_Window* CreateWindow(std::string windowName, int width, int height){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(windowName.c_str(), width, height, 0);
    return window;
}
