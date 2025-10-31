
#pragma once

#include <SDL.h>
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_impl_sdl.h>

#include <src/io/InputState.hpp>
#include <src/io/InputActionMapping.hpp>

using namespace std;

class InputManager
{
public:
    InputManager()
    {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    std::vector<Action> Update()
    {
        // Clear all leftover data from the last frame in the input state
        inputState.Reset();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // Check if ImGui wants to handle this event; if so, skip processing it
            auto& io = ImGui::GetIO();
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (io.WantCaptureMouse || io.WantCaptureKeyboard)
            {
                continue;
            }

            switch (event.type)
            {
                case SDL_KEYDOWN:
                {
                    if (event.key.repeat == 0)
                    {
                        SDL_Scancode sc = event.key.keysym.scancode;
                        inputState.down.set(sc);
                        inputState.pressed.set(sc);
                    }
                }
                break;

                case SDL_KEYUP:
                {
                    SDL_Scancode sc = event.key.keysym.scancode;
                    inputState.down.reset(sc);
                    inputState.released.set(sc);
                }
                break;

                case SDL_MOUSEBUTTONDOWN:
                {    
                    inputState.mouseDown |= SDL_BUTTON(event.button.button);
                    inputState.mousePressed |= SDL_BUTTON(event.button.button);
                }
                break;

                case SDL_MOUSEBUTTONUP:
                {
                    inputState.mouseDown &= ~SDL_BUTTON(event.button.button);
                    inputState.mouseReleased |= SDL_BUTTON(event.button.button);
                }
                break;

                case SDL_MOUSEWHEEL:
                {
                    inputState.wheelX += event.wheel.x;
                    inputState.wheelY += event.wheel.y;
                }
                break;

                case SDL_MOUSEMOTION:
                {
                    inputState.mouseX = event.motion.x;
                    inputState.mouseY = event.motion.y;

                    inputState.dx += event.motion.xrel;
                    inputState.dy += event.motion.yrel;
                }
                break;

                case SDL_QUIT:
                {
                    exit(0);
                }
                break;
            }
        }
        
        actions.clear();
        actions = MapInputToActions(inputState);

        return actions;
    }

private:
    InputState inputState;
    std::vector<Action> actions;
};
