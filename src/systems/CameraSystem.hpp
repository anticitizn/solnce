
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
        camera.position = {0, 0, 1.0f};
        camera.moveSpeed = 5.0f;
        camera.zoomSpeed = 1.1f;
        
        coordinator.RegisterResource<Camera>(camera);
    }

    void Update()
    {
        Camera& camera = coordinator.GetResource<Camera>();

        std::vector<Action> actions = coordinator.GetResource<std::vector<Action>>();
        for (const auto& action : actions)
        {
            if (action.type == MoveCamera && action.phase != Stopped)
            {
                camera.position.x += action.delta.x * camera.moveSpeed;
                camera.position.y += action.delta.y * camera.moveSpeed;
            }
            if (action.type == Zoom)
            {
                if (action.value > 0)
                {
                    camera.position.z *= camera.zoomSpeed;  // zoom in
                }
                else if (action.value < 0)
                {
                    camera.position.z /= camera.zoomSpeed;  // zoom out
                }
                    
                camera.position.z = glm::clamp(camera.position.z, 0.1f, 20.0f);
            }
            if (action.type == DragCamera && action.phase != Stopped)
            {
                camera.position.x -= action.delta.x / camera.position.z;
                camera.position.y -= action.delta.y / camera.position.z;
            }
        }

        camera.view = glm::mat4(1.0f);

        float w = camera.viewportSize.x;
        float h = camera.viewportSize.y;

        glm::vec2 cursorPos = coordinator.GetResource<CursorPos>().position;
        float half_w = (w * 0.5f) / camera.position.z;
        float half_h = (h * 0.5f) / camera.position.z;

        camera.projection = glm::ortho(camera.position.x - half_w, camera.position.x + half_w, 
                                       camera.position.y + half_h, camera.position.y - half_h, 
                                       -1.0f                     , 1.0f);
        camera.inverseProjection = glm::inverse(camera.projection);
    }
};


