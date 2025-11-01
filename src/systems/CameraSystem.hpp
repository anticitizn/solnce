
#pragma once

#include <string>
#include <iostream>
#include <unordered_set>

#include <src/components/Pos2D.hpp>
#include <src/components/Player.hpp>
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
    }
};


