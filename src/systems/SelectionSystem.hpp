
#pragma once

#include <string>
#include <iostream>

#include <external/glm/glm.hpp>

#include <src/components/Quad.hpp>
#include <src/ecs/System.hpp>
#include <src/components/Selected.hpp>
#include <src/components/Dragged.hpp>
#include <src/components/Transform.hpp>
#include <src/io/Action.hpp>
#include <src/resources/Camera.hpp>
#include <src/resources/BodySelection.hpp>

using namespace std;

extern Coordinator coordinator;

class SelectionSystem : public System
{
public:
    Signature InitialSignature()
    {
        return coordinator.BuildSignature<Quad, Transform>();
    }

    void Update()
    {
        std::vector<Action>& actions = coordinator.GetResource<std::vector<Action>>();
        auto& bodySelection = coordinator.GetResource<BodySelection>();

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
                    auto& tf = coordinator.GetComponent<Transform>(entity);

                    if (IsClicked(mouseX, mouseY, quad, tf))
                    {
                        anyEntityClicked = true;

                        bodySelection.selectedEntity = entity;
                    }
                }
                
                if (!anyEntityClicked)
                {
                    bodySelection.selectedEntity = INVALID_ENTITY;
                }
            }
        }
    }

private:
    bool IsClicked(float mouseX, float mouseY, const Quad& quad, const Transform& tf)
    {
        const Camera& camera = coordinator.GetResource<Camera>();
        glm::vec2 worldClick = ScreenToWorld(mouseX, mouseY, camera);

        // Compare click position in world space
        glm::vec2 quadCenter(tf.position.x, tf.position.y);
        glm::vec2 halfSize(quad.sizeX / 2.f, quad.sizeY / 2.f);

        // Translate to quad local space
        glm::vec2 local = worldClick - quadCenter;

        // Invert the rotation of the quad
        float rad = -glm::radians(tf.rotation);
        glm::mat2 rot(cos(rad), -sin(rad), sin(rad),  cos(rad));
        local = rot * local;

        return (local.x >= -halfSize.x && local.x <= halfSize.x &&
                local.y >= -halfSize.y && local.y <= halfSize.y);
    }

    glm::vec2 ScreenToWorld(float mouseX, float mouseY, const Camera& camera)
    {
        float ndcX = (2.0f * mouseX / camera.viewportSize.x) - 1.0f;
        float ndcY = 1.0f - (2.0f * mouseY / camera.viewportSize.y);

        glm::vec4 ndcPos(ndcX, ndcY, 0.0f, 1.0f);
        glm::vec4 localPos = camera.inverseProjection * ndcPos;
        localPos /= localPos.w;

        // Convert camera-local -> world
        return {
            localPos.x + (float)camera.position.x,
            localPos.y + (float)camera.position.y
        };
    }


};


