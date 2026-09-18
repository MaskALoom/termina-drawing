#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include <SDL3/SDL.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 550

#define RED (Color){255, 0, 0, 255}

class Camera{
private:
public:
    float x = 0.0f;
    float y = 0.0f;
    int width = 0;
    int height = 0;

    void MoveCamera(SDL_Event& event){
        const bool* keyboard = SDL_GetKeyboardState(nullptr);
        if(keyboard[SDL_SCANCODE_W]) y-=10.0f;
        if(keyboard[SDL_SCANCODE_A]) x-=10.0f;
        if(keyboard[SDL_SCANCODE_S]) y+=10.0f;
        if(keyboard[SDL_SCANCODE_D]) x+=10.0f;
    }
};

class Mouse{
private:
    float lastX = 0.0f;
    float lastY = 0.0f;

    float x = 0.0f;
    float y = 0.0f;
public:
    void MouseUpdate(void){
        lastX = x;
        lastY = y;
        SDL_GetMouseState(&x, &y);
    }
    void MoveCanvas(Camera& camera){
        const bool* keyboard = SDL_GetKeyboardState(nullptr);
        if(keyboard[SDL_SCANCODE_SPACE]){
            float xDifference = x - lastX;
            float yDifference = y - lastY;

            camera.x -= xDifference;
            camera.y -= yDifference;
        }
    }
};

class ProgramManager{
private:
public:
    bool running = true;
};

typedef struct{
    int r;
    int g;
    int b;
    int a;
}Color;

void DrawRectangle(float x, float y, float width, float height, Color color, SDL_Renderer* renderer, Camera& camera){
        SDL_FRect rect = {x-camera.x, y-camera.y, width, height};
        SDL_SetRenderDrawColor(renderer, color.a, color.b, color.g, color.a);
        SDL_RenderFillRect(renderer, &rect);
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

int main(int argc, char** argv){
    ProgramManager manager;
    Camera camera;
    Mouse mouse;
    SDL_Window* window = CreateWindow("Termina Drawing", WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_Renderer* renderer = CreateRenderer(window);
    if(!window || !renderer){
        SDL_Log("SDL Error (Fix that shit): %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    while(manager.running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                manager.running = false;
            }
            mouse.MouseUpdate();
            mouse.MoveCanvas(camera);
            camera.MoveCamera(event);
        }
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);
        DrawRectangle(100, 100, 200, 200, RED, renderer, camera);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}














