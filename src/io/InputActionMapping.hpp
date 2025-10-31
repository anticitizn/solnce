
#pragma once

#include <unordered_map>

#include "SDL.h"

#include <src/io/Action.hpp>

// Maps keyboard presses to logical actions
static const std::unordered_map<SDL_Scancode, ActionType> keyToAction = 
{
    {SDL_SCANCODE_W, ActionType::MoveCamera},
    {SDL_SCANCODE_S, ActionType::MoveCamera},
    {SDL_SCANCODE_A, ActionType::MoveCamera},
    {SDL_SCANCODE_D, ActionType::MoveCamera}
};

// Maps mouse button clicks to logical actions
static const std::unordered_map<uint8_t, ActionType> mouseToAction = 
{
    {SDL_BUTTON_LEFT, ActionType::Select},
    {SDL_BUTTON_MIDDLE, ActionType::DragCamera}
};

std::vector<Action> MapInputToActions(const InputState& input)
{
    std::vector<Action> actions;

    // Keyboard
    for (int sc = 0; sc < SDL_NUM_SCANCODES; ++sc)
    {
        if (input.pressed.test(sc))
        {
            auto it = keyToAction.find((SDL_Scancode)sc);
            if (it != keyToAction.end())
            {
                actions.push_back( {it->second, ActionPhase::Started, {}, {}, 0.0f} );
            }
        }

        if (input.down.test(sc))
        {
            auto it = keyToAction.find((SDL_Scancode)sc);
            if (it != keyToAction.end())
            {
                actions.push_back( {it->second, ActionPhase::Held, {}, {}, 0.0f} );
            }
        }

        if (input.released.test(sc))
        {
            auto it = keyToAction.find((SDL_Scancode)sc);
            if (it != keyToAction.end())
            {
                actions.push_back( {it->second, ActionPhase::Stopped, {}, {}, 0.0f} );
            }
        }
    }

    // Mouse buttons
    for (auto [button, actionType] : mouseToAction)
    {
        uint32_t mask = SDL_BUTTON(button);
        if (input.mousePressed & mask)
        {
            actions.push_back( {actionType, ActionPhase::Started, {input.mouseX, input.mouseY}, {}, 0.0f} );
        }
        else if (input.mouseDown & mask)
        {
            actions.push_back( {actionType, ActionPhase::Held, {input.mouseX, input.mouseY}, {input.dx, input.dy}, 0.0f} );
        }
        else if (input.mouseReleased & mask)
        {
            actions.push_back( {actionType, ActionPhase::Stopped, {input.mouseX, input.mouseY}, {}, 0.0f} );
        }
    }

    return actions;
}