
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
        camera.position.z = 0.9;
        coordinator.RegisterResource<Camera>(camera);
    }

    void Update()
    {
        for (const auto& entity : entities)
        {
            auto& pos2d = coordinator.GetComponent<Pos2D>(entity);
            auto& player = coordinator.GetComponent<Player>(entity);

            // Reset velocity every frame
            player.velocity = {0, 0};

            std::vector<Action> actions = coordinator.GetResource<std::vector<Action>>();
            for (const auto& action : actions)
            {
                if (action.type == MoveCamera && action.phase != Stopped)
                {
                    player.velocity.x += action.delta.x * player.speed;
                    player.velocity.y += action.delta.y * player.speed;
                }
                if (action.type == Zoom)
                {
                    float zoomSpeed = 1.1f;
                    if (action.value > 0)
                    {
                        pos2d.posZ *= zoomSpeed;  // zoom in
                    }
                    else if (action.value < 0)
                    {
                        pos2d.posZ /= zoomSpeed;  // zoom out
                    }
                        
                    pos2d.posZ = glm::clamp(pos2d.posZ, 0.1f, 20.0f);
                }
                if (action.type == DragCamera && action.phase != Stopped)
                {
                    pos2d.posX -= action.delta.x / pos2d.posZ;
                    pos2d.posY -= action.delta.y / pos2d.posZ;
                }
            }

            pos2d.posX += player.velocity.x;
            pos2d.posY += player.velocity.y;

            Camera camera = coordinator.GetResource<Camera>();

            camera.position = glm::vec3(pos2d.posX, pos2d.posY, pos2d.posZ);

            coordinator.SetResource<Camera>(camera);
        }
    }
};


