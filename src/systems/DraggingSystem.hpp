
#pragma once

#include <string>
#include <iostream>

#include <src/components/Quad.hpp>

#include <src/ecs/System.hpp>

using namespace std;

extern Coordinator coordinator;

class DraggingSystem : public System
{
public:
    void Update()
    {
        for (const auto& entity : entities)
        {
            auto& quad = coordinator.GetComponent<Quad>(entity);
            const Camera& camera = coordinator.GetResource<Camera>();

            std::vector<Action> actions = coordinator.GetResource<std::vector<Action>>();
            for (const auto& action : actions)
            {
                if (action.type == Select)
                {
                    quad.posX += action.delta.x / camera.position.z;
                    quad.posY += action.delta.y / camera.position.z;
                }
            }
        }
    }
};


