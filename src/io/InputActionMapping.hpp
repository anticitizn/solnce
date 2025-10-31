
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

// Helper to compute WASD axis from InputState::down (held keys)
static inline glm::vec2 ComputeMoveAxis(const InputState& input) 
{
    float x = 0.f, y = 0.f;

    if (input.down.test(SDL_SCANCODE_D)) 
    {
        x += 1.f;
    }
    if (input.down.test(SDL_SCANCODE_A))
    {
        x -= 1.f;
    }
    
    if (input.down.test(SDL_SCANCODE_W)) 
    {
        y -= 1.f;
    }
    
    if (input.down.test(SDL_SCANCODE_S))
    {
        y += 1.f;
    }

    // Normalize diagonals so speed is consistent
    if (x != 0.f || y != 0.f) 
    {
        float len = std::sqrt(x*x + y*y);
        x /= len; 
        y /= len;
    }
    return {x, y};
}


std::vector<Action> MapInputToActions(const InputState& input)
{
    static glm::vec2 prevAxis{0.f, 0.f};
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

    if (input.dx != 0.0f || input.dy != 0.0f)
    {
        actions.push_back( {ActionType::CursorMotion, ActionPhase::Held, {input.mouseX, input.mouseY}, {input.dx, input.dy}, 0.0f} );
    }

    // Calculate delta for WASD MoveCamera actions
    glm::vec2 axis = ComputeMoveAxis(input);
    bool wasZero = prevAxis.x == 0.f && prevAxis.y == 0.f;
    bool isZero  = axis.x == 0.f && axis.y == 0.f;

    if (!isZero && wasZero) 
    {
        actions.push_back({ ActionType::MoveCamera, ActionPhase::Started, {}, axis, 0.0f });
    } 
    else if (!isZero && !wasZero) 
    {
        actions.push_back({ ActionType::MoveCamera, ActionPhase::Held,    {}, axis, 0.0f });
    } 
    else if (isZero && !wasZero) 
    {
        actions.push_back({ ActionType::MoveCamera, ActionPhase::Stopped, {}, {},   0.0f });
    }

    prevAxis = axis;

    if (input.wheelY != 0)
    {
        actions.push_back( { ActionType::Zoom, ActionPhase::Started, {}, {}, (float)input.wheelY} );
    }

    return actions;
}