
#pragma once

#include <string>
#include <iostream>

#include <src/components/Quad.hpp>

#include <src/ecs/System.hpp>

using namespace std;

extern Coordinator coordinator;

class MovingSystem : public System, public InputEventListener
{
public:
    void Update()
    {
        for (const auto& entity : entities)
        {
            auto& quad = coordinator.GetComponent<Quad>(entity);
            for (const auto& event : *events)
            {
                if (event.type == SDL_MOUSEMOTION)
                {
                    quad.posX += event.motion.xrel;
                    quad.posY += event.motion.yrel;
                }
            }
        }
    }
};


