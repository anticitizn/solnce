
#pragma once

#include <string>
#include <iostream>
#include <unordered_set>

#include <src/components/Pos2D.hpp>
#include <src/ecs/System.hpp>

#include <external/glm/glm.hpp>

using namespace std;

extern Coordinator coordinator;

class CameraSystem : public System
{
public:
    CameraSystem()
    {
        Camera camera;
        camera.position = {0, 0};
        
        coordinator.RegisterResource<Camera>(camera);
    }

    Signature InitialSignature()
    {
        return coordinator.BuildSignature<Pos2D>();
    }

    void Update()
    {
        Camera& camera = coordinator.GetResource<Camera>();
        std::vector<Action> actions = coordinator.GetResource<std::vector<Action>>();

        for (const auto& action : actions)
        {
            if (action.type == MoveCamera && action.phase != Stopped)
            {
                glm::dvec2 deltaPx(action.delta.x, action.delta.y);
                glm::dvec2 deltaMeters = deltaPx * camera.metersPerPixel * camera.moveSpeed;
                camera.position += deltaMeters;
            }

            if (action.type == Zoom)
            {
                if (action.value > 0)
                {
                    camera.metersPerPixel *= camera.zoomSpeed;  // zoom in
                }
                else if (action.value < 0)
                {
                    camera.metersPerPixel /= camera.zoomSpeed;  // zoom out
                }
                    
                camera.metersPerPixel = glm::clamp(camera.metersPerPixel, camera.minMetersPerPixel, camera.maxMetersPerPixel);
            }

            if (action.type == DragCamera && action.phase != Stopped)
            {
                glm::dvec2 dragPx(action.delta.x, action.delta.y);
                camera.position -= dragPx * camera.metersPerPixel;
            }
        }

        camera.view = glm::mat4(1.0f);

        double w = camera.viewportSize.x;
        double h = camera.viewportSize.y;

        double half_w = 0.5 * w * camera.metersPerPixel;
        double half_h = 0.5 * h * camera.metersPerPixel;

        camera.view = glm::mat4(1.0f);
        camera.projection = glm::ortho((float)(camera.position.x - half_w), (float)(camera.position.x + half_w), 
                                       (float)(camera.position.y + half_h), (float)(camera.position.y - half_h), 
                                       -1.0f                     , 1.0f);
        camera.inverseProjection = glm::inverse(camera.projection);
    }
};


