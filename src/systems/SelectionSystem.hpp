
#pragma once

#include <string>
#include <iostream>

#include <external/glm/glm.hpp>

#include <src/components/Quad.hpp>
#include <src/ecs/System.hpp>
#include <src/components/Selected.hpp>
#include <src/components/Dragged.hpp>
#include <src/io/Action.hpp>
#include <src/resources/Camera.hpp>

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
    bool IsClicked(float mouseX, float mouseY, const Quad& quad)
    {
        const Camera& camera = coordinator.GetResource<Camera>();
        glm::vec2 worldClick = ScreenToWorld(mouseX, mouseY, camera);

        // Compare click position in world space
        glm::vec2 quadCenter(quad.posX, quad.posY);
        glm::vec2 halfSize(quad.sizeX / 2.f, quad.sizeY / 2.f);

        // Translate to quad local space
        glm::vec2 local = worldClick - quadCenter;

        // Invert the rotation of the quad
        float rad = -glm::radians(quad.rot);
        glm::mat2 rot(cos(rad), -sin(rad), sin(rad),  cos(rad));
        local = rot * local;

        return (local.x >= -halfSize.x && local.x <= halfSize.x &&
                local.y >= -halfSize.y && local.y <= halfSize.y);
    }

    glm::vec2 ScreenToWorld(float mouseX, float mouseY, const Camera& camera)
    {
        // Convert position to NDC (-1 to +1)
        float ndcX = (2.0f * mouseX / camera.viewportSize.x) - 1.0f;
        float ndcY = 1.0f - (2.0f * mouseY / camera.viewportSize.y); // flip Y

        glm::vec4 ndcPos(ndcX, ndcY, 0.0f, 1.0f);

        // Transform to world position using the inverted projection matrix
        glm::vec4 worldPos = camera.inverseProjection * ndcPos;
        worldPos /= worldPos.w;

        return { worldPos.x, worldPos.y };
    }

};


