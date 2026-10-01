#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
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
//#include "../include/drawManager.hpp"
#include "../include/canvas.hpp"

SDL_Renderer* CreateRenderer(SDL_Window* window);
SDL_Window* CreateWindow(std::string windowName, int width, int height);

class Program{
private:
public:
    Camera camera;
    Mouse mouse;
    StrokeHandler strokeHandler;
    Canvas canvas;
    CanvasInputHandler cInputHandler;

    SDL_Window* window;
    SDL_Renderer* renderer;

    bool programExit = false;
    bool running = true;
    
    void ProgramInit(void){
        window = CreateWindow("Termina Drawing", WINDOW_WIDTH, WINDOW_HEIGHT);
        renderer = CreateRenderer(window);

        if(!window || !renderer){
            SDL_Log("SDL Error (Fix that shit): %s", SDL_GetError());
            SDL_Quit();
            programExit = true;

            return;
        }
        camera.Init();
        canvas.CanvasInit(2500, 2000, GetMonochromeColor((char)200));
        canvas.renderer = renderer;
        canvas.camera = &camera;

        strokeHandler.Init(8, 1, 0.5f, &canvas);
        canvas.strokeHandler = &strokeHandler;
        cInputHandler.Init(&canvas, &strokeHandler);
    }
    void EventPollHandler(SDL_Event& event){
        ResetGlobalKeysAndButtons();
        std::vector<SDL_Event> events;
        while(SDL_PollEvent(&event)) events.push_back(event);
        for(const SDL_Event& eventThing : events){
            if(eventThing.type == SDL_EVENT_QUIT){
                running = false;
            }
            else if(eventThing.type == SDL_EVENT_KEY_DOWN) globalKeyPressed = eventThing.key.key;
            else if(eventThing.type == SDL_EVENT_MOUSE_BUTTON_DOWN) globalButtonPressed = eventThing.button.button;
            else if(eventThing.type == SDL_EVENT_MOUSE_BUTTON_UP) globalButtonReleased = eventThing.button.button;
            else if(eventThing.type == SDL_EVENT_PEN_DOWN) globalPenHeld = true;
            else if(eventThing.type == SDL_EVENT_PEN_UP) globalPenReleased = true;
            else continue;
        }
        mouse.CheckActiveInputDevice();
        for(SDL_Event& eventThing : events){
            if(eventThing.type == SDL_EVENT_QUIT){
                running = false;
            }
            mouse.GetMousePath(eventThing);
        }
        events.clear();
    }
    void Update(void){
        mouse.MouseUpdate(camera);
        cInputHandler.Update();

        SDL_Event event;
        EventPollHandler(event);
        strokeHandler.MousePathStrokeInterpolation(mouse);
        camera.AlterZoom();
        camera.MoveCamera(mouse.GetMouseDifference());

        camera.state = CameraState::STATE_NONE;
    }
    void Draw(void){
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);
        canvas.DrawCanvasAndStrokes();
        SDL_RenderPresent(renderer);
    }
};

int main(int argc, char** argv){
    Program main;
    main.ProgramInit();
    while(main.running){
        if(main.programExit) break;
        else{
            main.Update();
            main.Draw();
        }
    }
    SDL_DestroyRenderer(main.renderer);
    SDL_DestroyWindow(main.window);
    SDL_Quit();
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
