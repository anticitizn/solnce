#include "WindowManager.hpp"

#include <iostream>

#include <external/imgui/imgui.h>
#include <external/imgui/imgui_impl_sdl.h>
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

    Uint32 windowedFlag = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_SHOWN;
	sdl_window = SDL_CreateWindow("Solnce", windowPosX, windowPosY, windowSizeX, windowSizeY, SDL_WINDOW_OPENGL | windowedFlag);
	SDL_MaximizeWindow(sdl_window);
	
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

	SDL_SetRelativeMouseMode(SDL_FALSE);

	SDL_GetWindowSize(sdl_window, &contextWidth, &contextHeight);
	cout << "Resolution is " << contextWidth << "/" << contextHeight << endl;

	IMGUI_CHECKVERSION();
    ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(sdl_window, context);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void WindowManager::Refresh()
{
    
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		if (event.type == SDL_QUIT)
			Exit();
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(sdl_window))
			Exit();
	}	
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

int WindowManager::GetContextWidth()
{
	SDL_GetWindowSize(sdl_window, &contextWidth, &contextHeight);
	return contextWidth;
}

int WindowManager::GetContextHeight()
{
	SDL_GetWindowSize(sdl_window, &contextWidth, &contextHeight);
	return contextHeight;
}