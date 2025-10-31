
#pragma once

#include <string>
#include <iostream>

#include <external/glm/glm.hpp>

#include <src/components/Quad.hpp>
#include <src/ecs/System.hpp>
#include <src/components/Selected.hpp>
#include <src/components/Dragged.hpp>
#include <src/io/Action.hpp>
#include <src/context/Camera.hpp>

using namespace std;

extern Coordinator coordinator;

class SelectionSystem : public System
{
public:
    void Update()
    {
        std::vector<Action> actions = coordinator.GetResource<std::vector<Action>>();

        for (const auto& action : actions)
        {
            if (action.type == Select && action.phase == Started)
            {
                int mouseX, mouseY;
                mouseX = action.position.x;
                mouseY = action.position.y;

                // If empty space was clicked, we remove all entities from the selection
                bool anyEntityClicked = false;

                for (const auto& entity : entities)
                {
                    auto& quad = coordinator.GetComponent<Quad>(entity);

                    if (IsClicked(mouseX, mouseY, quad))
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
            else if (action.type == Select && action.phase == Stopped)
            {
                int mouseX, mouseY;
                mouseX = action.position.x;
                mouseY = action.position.y;
                
                for (const auto& entity : entities)
                {
                    coordinator.RemoveComponent<Dragged>(entity);
                }
            }
        }
    }

private:
    bool IsClicked(const float mouseX, const float mouseY, struct Quad quad)
    {
        Camera camera = coordinator.GetResource<Camera>();
        
        // Move vector origin to the center of the quad
        float translatedX = mouseX - quad.posX + camera.position[0];
        float translatedY = mouseY - quad.posY + camera.position[1];

        // Inverse the quad's rotation
        float angleRad = -quad.rot * (M_PI / 180.0f);
        float cosAngle = cosf(angleRad);
        float sinAngle = sinf(angleRad);

        // Restore vector origin
        float rotatedX = translatedX * cosAngle - translatedY * sinAngle;
        float rotatedY = translatedX * sinAngle + translatedY * cosAngle;

        // Check if the point is inside the "unrotated" quad
        float halfWidth = quad.sizeX / 2.0f;
        float halfHeight = quad.sizeY / 2.0f;

        return (rotatedX >= -halfWidth && rotatedX <= halfWidth &&
                rotatedY >= -halfHeight && rotatedY <= halfHeight);
    }
};


