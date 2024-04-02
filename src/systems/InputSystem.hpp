
#pragma once

#include <string>
#include <iostream>
#include <list>

#include <SDL.h>

#include <src/InputEventListener.hpp>
#include <src/components/Quad.hpp>
#include <src/ecs/System.hpp>
#include <src/components/Selected.hpp>
#include <src/components/Dragged.hpp>

using namespace std;

extern Coordinator coordinator;

class InputSystem : public System, public InputEventListener
{
public:
    void Update()
    {
        for (const auto& event : *events)
        {
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // If empty space was clicked, we remove all entities from the selection
                bool anyEntityClicked = false;

                for (const auto& entity : entities)
                {
                    auto& quad = coordinator.GetComponent<Quad>(entity);

                    if (IsClicked(mouseX, mouseY, quad.posX, quad.posY, quad.sizeX, quad.sizeY, quad.rot))
                    {
                        anyEntityClicked = true;

                        coordinator.AddComponent<Selected>(entity, Selected {});
                        coordinator.AddComponent<Dragged>(entity, Dragged {});
                    }
                }
                
                if (!anyEntityClicked)
                {
                    for (const auto& entity : entities)
                    {
                        coordinator.RemoveComponent<Selected>(entity);
                    }
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                
                for (const auto& entity : entities)
                {
                    coordinator.RemoveComponent<Dragged>(entity);
                }
            }
        }
    }

private:
    bool IsClicked(const float mouseX, const float mouseY, const float posX, const float posY, const float sizeX, const float sizeY, const float rot)
    {
        // Move vector origin to the center of the quad
        float rotMouseX = mouseX - posX;
        float rotMouseY = mouseY - posY;

        // Rotate vector by the quad's rotation in the inverse direction
        rotMouseX = cos(-rot)*rotMouseX - sin(-rot)*rotMouseY;
        rotMouseY = sin(-rot)*rotMouseX + cos(-rot)*rotMouseY;

        // Restore vector origin
        rotMouseX += posX;
        rotMouseY += posY;

        // Check if the rotated mouse click coordinates are within the quad
        if ((rotMouseX < posX + sizeX && rotMouseX > posX - sizeX) && (rotMouseY < posY + sizeY && rotMouseY > posY - sizeY))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};


