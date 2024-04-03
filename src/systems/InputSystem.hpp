
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
        float rotMouseX = mouseX;
        float rotMouseY = mouseY;

        // If the rectangle is rotated, we have to rotate the click point 
        // around the quad's center
        if (rot != 0)
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
        }

        // Preliminary check: see if clicked point is within the rectangle's axis-aligned bounding box
        if ((rotMouseX < posX + sizeX/2 && rotMouseX > posX - sizeX/2) && (rotMouseY < posY + sizeY/2 && rotMouseY > posY - sizeY/2))
        {
            // TO-DO: Check if point within quad
            return true;
        }
        else
        {
            return false;
        }
    }
};


