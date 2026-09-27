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
#include "../include/drawManager.hpp"

SDL_Renderer* CreateRenderer(SDL_Window* window);
SDL_Window* CreateWindow(std::string windowName, int width, int height);

class Program{
private:
public:
    Camera camera;
    Mouse mouse;
    DrawManager drawManager;

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

        drawManager.renderer = renderer;
        drawManager.camera = &camera;
        drawManager.Init(2500, 2000, GetMonochromeColor((char)230));
        drawManager.CreateRect(100, 100, 200, 200, RED);
        camera.Init();
    }
    void EventPollHandler(SDL_Event& event){
        globalKeyPressed = SDLK_UNKNOWN;
        int motions = 0;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
            else if(event.type == SDL_EVENT_KEY_DOWN){
                globalKeyPressed = event.key.key;
            }
            else if(event.type == SDL_EVENT_MOUSE_MOTION){
                if(mouse.isDrawing) mouse.mousePathBuffer.push_back({event.motion.x, event.motion.y});
                ++motions;
            }
        }
        if(motions > 0){
            //std::cout << "MOTIONS: " << motions << std::endl;
        }
    }
    void Update(void){
        mouse.MouseUpdate();

        SDL_Event event;
        EventPollHandler(event);
        drawManager.Update(mouse);
        camera.AlterZoom();
        camera.MoveCamera(mouse.GetMouseDifference());

        camera.state = CameraState::STATE_NONE;
    }
    void Draw(void){
        SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
        SDL_RenderClear(renderer);
        drawManager.Draw();
        SDL_RenderPresent(renderer);
    }
};

int main(int argc, char** argv){
    Program main;
    main.ProgramInit();
    while(main.running){
        if(main.programExit) break;
        main.Update();
        main.Draw();
    }
    SDL_DestroyRenderer(main.renderer);
    SDL_DestroyWindow(main.window);
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
