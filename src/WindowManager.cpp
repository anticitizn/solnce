#include "WindowManager.hpp"

#include <string>
#include <iostream>

using namespace std;

// Caution - parameters have default values set in header
void WindowManager::Init(string windowName, bool fullscreen, unsigned int windowSizeX, unsigned int windowSizeY,
                         unsigned int windowPosX, unsigned int windowPosY)
{
    cout << "Initializing SDL" << endl;

	if (SDL_Init(SDL_INIT_VIDEO || SDL_INIT_AUDIO) == -1)
	{
		cout << "SDL initialization failed " << SDL_GetError();
		exit(-1);
	}

	atexit(SDL_Quit);

    Uint32 windowedFlag = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_SHOWN;
	window = SDL_CreateWindow("SDL Window", windowPosX, windowPosY, windowSizeX, windowSizeY, SDL_WINDOW_OPENGL | windowedFlag);
	SDL_MaximizeWindow(window);
	
	if (window == NULL)
	{
		cout << "SDL couldn't set video mode" << endl << SDL_GetError() << endl;
		exit(-1);
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	cout << "SDL initialized" << endl;

	// Initializing GLAD, make sure it's after the OpenGL context initialization
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		exit(-1);
	}

	SDL_SetRelativeMouseMode(SDL_FALSE);

	SDL_GetWindowSize(window, &contextWidth, &contextHeight);
	cout << "Resolution is " << contextWidth << "/" << contextHeight << endl;
}

void WindowManager::Refresh()
{
    SDL_GL_SwapWindow(window);
}

void WindowManager::Exit()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int WindowManager::GetContextWidth()
{
	return contextWidth;
}


int WindowManager::GetContextHeight()
{
	return contextHeight;
}