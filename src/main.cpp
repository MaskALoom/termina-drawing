#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include <SDL3/SDL.h>

class ProgramManager{
private:
public:
    bool running = true;
};

SDL_Window* CreateWindow(std::string windowName, int width, int height){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(windowName.c_str(), width, height, 0);
    if(!window) return nullptr;
    else return window;
}

int main(int argc, char** argv){
    ProgramManager manager;
    SDL_Window* window = CreateWindow("Termina Drawing", 1000, 750);
    if(!window){
        SDL_Log("Failed to create window and all that: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    while(manager.running){
        SDL_Event event;
    }

    return 0;
}














