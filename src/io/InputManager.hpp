
#pragma once

#include <SDL3/SDL.h>
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_impl_sdl3.h>

#include <src/io/InputState.hpp>
#include <src/io/InputActionMapping.hpp>

using namespace std;

class InputManager
{
public:
    InputManager()
    {

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
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (io.WantCaptureMouse || io.WantCaptureKeyboard)
            {
                continue;
            }

            switch (event.type)
            {
                case SDL_EVENT_KEY_DOWN:
                {
                    if (event.key.repeat == 0)
                    {
                        SDL_Scancode sc = event.key.scancode;
                        inputState.down.set(sc);
                        inputState.pressed.set(sc);
                    }
                }
                break;

                case SDL_EVENT_KEY_UP:
                {
                    SDL_Scancode sc = event.key.scancode;
                    inputState.down.reset(sc);
                    inputState.released.set(sc);
                }
                break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {    
                    inputState.mouseDown |= SDL_BUTTON_MASK(event.button.button);
                    inputState.mousePressed |= SDL_BUTTON_MASK(event.button.button);
                }
                break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                {
                    inputState.mouseDown &= ~SDL_BUTTON_MASK(event.button.button);
                    inputState.mouseReleased |= SDL_BUTTON_MASK(event.button.button);
                }
                break;

                case SDL_EVENT_MOUSE_WHEEL:
                {
                    inputState.wheelX += event.wheel.x;
                    inputState.wheelY += event.wheel.y;
                }
                break;

                case SDL_EVENT_MOUSE_MOTION:
                {
                    inputState.mouseX = event.motion.x;
                    inputState.mouseY = event.motion.y;

                    inputState.dx += event.motion.xrel;
                    inputState.dy += event.motion.yrel;
                }
                break;

                case SDL_EVENT_QUIT:
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

    InputState GetInputState()
    {
        return inputState;
    }

private:
    InputState inputState;
    std::vector<Action> actions;
};
