
#pragma once

#include <string>
#include <iostream>

#include <src/components/Quad.hpp>
#include <src/components/Transform.hpp>

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
            auto& tf = coordinator.GetComponent<Transform>(entity);
            const Camera& camera = coordinator.GetResource<Camera>();

            std::vector<Action> actions = coordinator.GetResource<std::vector<Action>>();
            for (const auto& action : actions)
            {
                if (action.type == Select)
                {
                    tf.position.x += action.delta.x / camera.position.z;
                    tf.position.y += action.delta.y / camera.position.z;
                }
            }
        }
    }
};


