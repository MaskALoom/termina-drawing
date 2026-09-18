#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include <SDL3/SDL.h>

#include <vector>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 550

#define RED (Color){255, 0, 0, 255}

enum class ProgramState{
    STATE_ZOOM_CHANGE = 0,
    NONE
};

ProgramState gState;

class Camera{
private:
public:
    float x = 0.0f;
    float y = 0.0f;
    int width = 0;
    int height = 0;

    float zoom = 1.0f;

    void AlterZoom(void){
        const bool* keyboard = SDL_GetKeyboardState(nullptr);
        if(keyboard[SDL_SCANCODE_Q]){
            zoom -= 0.1f;
        }
        else if(keyboard[SDL_SCANCODE_W]){
            zoom += 0.1f;
        }

        if(zoom <= 0.0f) zoom = 0.1f;
        else if(zoom >= 5.0f) zoom = 5.0f;
    }
    void MoveCamera(void){
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
        //return;
        const bool* keyboard = SDL_GetKeyboardState(nullptr);
        if(keyboard[SDL_SCANCODE_SPACE]){
            //if(!keyboard[SDL_SCANCODE_E]) return;
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

typedef struct{
    float x;
    float y;
    int width;
    int height;
    Color color;
}Rectangle;

class DrawManager{
public:
    SDL_Renderer* renderer;
    Camera* camera;
    std::vector<Rectangle> rects;
    void Update(void){
        if(gState == ProgramState::STATE_ZOOM_CHANGE){
            for(auto& rect : rects){
            }
        }
    }
    void CreateRect(float x, float y, int width, int height, Color color){
        rects.push_back((Rectangle){x, y, width, height, color});
    }
    void Draw(void){
        //if(vector.empty()) return;
        for(auto& rect : rects){
            DrawRectangle(rect, renderer, *camera);
        }
    }
    void DrawRectangle(Rectangle& rect, SDL_Renderer* renderer, Camera& camera){
        float newX = rect.x-camera.x;
        float newY = rect.y-camera.y;
        float newWidth = rect.width*camera.zoom;
        float newHeight = rect.height*camera.zoom;

        SDL_FRect newRect = {newX, newY, newWidth, newHeight};
        SDL_SetRenderDrawColor(renderer, rect.color.a, rect.color.b, rect.color.g, rect.color.a);
        SDL_RenderFillRect(renderer, &newRect);
    }
};

void DrawRectangle(float x, float y, float width, float height, Color color, SDL_Renderer* renderer, Camera& camera){
    //float newX = x-camera.x*camera.zoom;
    //float newY = y-camera.y*camera.zoom;

    float newX = x-camera.x;
    float newY = y-camera.y;
    float newWidth = width*camera.zoom;
    float newHeight = height*camera.zoom;

    SDL_FRect rect = {newX, newY, newWidth, newHeight};
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
    gState = ProgramState::NONE;

    ProgramManager manager;
    DrawManager drawManager;
    drawManager.CreateRect(0, 0, 200, 200, RED);
    drawManager.CreateRect(400, 400, 200, 200, RED);
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
            camera.AlterZoom();
            mouse.MouseUpdate();
            mouse.MoveCanvas(camera);
            //camera.MoveCamera();
        }
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);
        drawManager.Draw();
        //DrawRectangle(100, 100, 200, 200, RED, renderer, camera);
        SDL_RenderPresent(renderer);

        gState = ProgramState::NONE;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}














