
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
        const Camera& camera = coordinator.GetResource<Camera>();

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

                    if (IsClicked(mouseX, mouseY, quad, tf, camera))
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
    bool IsClicked(double mouseX, double mouseY, const Quad& quad, const Transform& tf, const Camera& camera)
    {
        glm::dvec2 worldClick = ScreenToWorld(mouseX, mouseY, camera);

        double minPx = 20.0;
        double minSize_m = minPx * camera.metersPerPixel;

        double sx_m = std::max((double)quad.sizeX, minSize_m);
        double sy_m = std::max((double)quad.sizeY, minSize_m);

        glm::dvec2 quadCenter(tf.position.x, tf.position.y);
        glm::dvec2 halfSize(sx_m * 0.5, sy_m * 0.5);

        glm::dvec2 local = worldClick - quadCenter;

        double rad = -glm::radians((double)tf.rotation);
        glm::dmat2 rot(cos(rad), -sin(rad), sin(rad), cos(rad));
        local = rot * local;

        return (local.x >= -halfSize.x && local.x <= halfSize.x &&
                local.y >= -halfSize.y && local.y <= halfSize.y);
    }


    glm::dvec2 ScreenToWorld(double mouseX, double mouseY, const Camera& camera)
    {
        double ndcX = (2.0 * mouseX / camera.viewportSize.x) - 1.0;
        double ndcY = 1.0 - (2.0 * mouseY / camera.viewportSize.y);

        glm::dvec4 ndcPos(ndcX, ndcY, 0.0, 1.0);

        // camera.inverseProjection is mat4 (float). Prefer a double inverseProjection if possible.
        glm::vec4 camPos_f = camera.inverseProjection * glm::vec4((float)ndcX, (float)ndcY, 0.0f, 1.0f);
        camPos_f /= camPos_f.w;

        glm::dvec2 camPos(camPos_f.x, camPos_f.y);   // camera-space meters
        return camPos + camera.position;             // world-space meters
    }


};


