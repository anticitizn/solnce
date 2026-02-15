
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
    Signature InitialSignature()
    {
        return coordinator.BuildSignature<Quad, Dragged>();
    }

    void Update()
    {
        const Camera& camera = coordinator.GetResource<Camera>();

        for (const auto& entity : entities)
        {
            auto& quad = coordinator.GetComponent<Quad>(entity);
            auto& tf = coordinator.GetComponent<Transform>(entity);

            std::vector<Action> actions = coordinator.GetResource<std::vector<Action>>();
            for (const auto& action : actions)
            {
                if (action.type == Select)
                {
                    tf.position.x += action.delta.x / camera.metersPerPixel;
                    tf.position.y += action.delta.y / camera.metersPerPixel;
                }
            }
        }
    }
};


