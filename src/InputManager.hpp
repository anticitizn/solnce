
#pragma once

#include <list>

#include <SDL.h>
#include <src/MouseState.hpp>

using namespace std;

class InputManager
{
public:
    InputManager()
    {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    void Update()
    {
        // Prevent the list of pending events from getting filled with
        // events that no one is interested in
        // Events should usually be handled within a single frame so this should be OK
        if (events.size() > 0)
        {
            events.clear();
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            auto& io = ImGui::GetIO();
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (io.WantCaptureMouse || io.WantCaptureKeyboard)
            {
                continue;
            }

            if(event.type == SDL_QUIT)
            {
                exit(0);
            }

            events.push_back(event);
        }

        uint32_t mouseStateBitmask = SDL_GetMouseState(&mouseState.x, &mouseState.y);
        mouseState.lmb = mouseStateBitmask & SDL_BUTTON_LMASK;
        mouseState.mmb = mouseStateBitmask & SDL_BUTTON_MMASK;
        mouseState.rmb = mouseStateBitmask & SDL_BUTTON_RMASK;
    }

    list<SDL_Event>* GetEvents()
    {
        return &events;
    }

    MouseState& GetMouseState()
    {
        return mouseState;
    }

private:
    list<SDL_Event> events;
    MouseState mouseState;
};
