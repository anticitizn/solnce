
#pragma once

#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <external/glad/glad.h>

#include <src/ui/Window.hpp>

using namespace std;

class WindowManager
{
public:      
    void Init(
        string windowName, bool fullscreen, unsigned int windowSizeX=1200, unsigned int windowSizeY=800, 
        unsigned int windowPosX=100, unsigned int WindowPosY=100);
    void Refresh();
    void Exit();
    int GetContextWidth();
    int GetContextHeight();
    void SwapBuffers();

private:
    SDL_Window* sdl_window;
    int contextWidth;
    int contextHeight;
};
