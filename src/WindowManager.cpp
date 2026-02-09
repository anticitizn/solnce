#include "WindowManager.hpp"

#include <iostream>

#include <external/imgui/imgui.h>
#include <external/imgui/imgui_impl_sdl3.h>
#include <external/imgui/imgui_impl_opengl3.h>

using namespace std;

// Caution - parameters have default values set in header
void WindowManager::Init(string windowName, bool fullscreen, unsigned int windowSizeX, unsigned int windowSizeY,
                         unsigned int windowPosX, unsigned int windowPosY)
{
    cout << "Initializing SDL" << endl;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == -1)
	{
		cout << "SDL initialization failed " << SDL_GetError();
		exit(-1);
	}

	atexit(SDL_Quit);

	// Enable 4x MSAA
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    Uint32 windowedFlag = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
	sdl_window = SDL_CreateWindow("Solnce", windowSizeX, windowSizeY, SDL_WINDOW_OPENGL | windowedFlag);
	SDL_SetWindowFullscreen(sdl_window, true);
	
	if (sdl_window == NULL)
	{
		cout << "SDL couldn't set video mode" << endl << SDL_GetError() << endl;
		exit(-1);
	}

	SDL_GLContext context = SDL_GL_CreateContext(sdl_window);
	cout << "SDL initialized" << endl;

	// Initializing GLAD, make sure it's after the OpenGL context initialization
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		exit(-1);
	}


	SDL_GetWindowSizeInPixels(sdl_window, &contextWidth, &contextHeight);
	cout << "Resolution is " << contextWidth << "/" << contextHeight << endl;

	SDL_CaptureMouse(true);

	IMGUI_CHECKVERSION();
    ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplSDL3_InitForOpenGL(sdl_window, context);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void WindowManager::Refresh()
{
    
}

void WindowManager::SwapBuffers()
{
	SDL_GL_SwapWindow(sdl_window);
}

void WindowManager::Exit()
{
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
}

void WindowManager::GetContextSize(int* width, int* height)
{
	SDL_GetWindowSizeInPixels(sdl_window, width, height);
}
