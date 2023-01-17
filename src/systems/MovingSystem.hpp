
#pragma once

#include <string>
#include <iostream>

#include <src/components/Quad.hpp>

#include <src/ecs/System.hpp>

using namespace std;

extern Coordinator coordinator;

class MovingSystem : public System
{
public:
    void Update()
    {
        for (const auto& entity : entities)
        {
            auto& quad = coordinator.GetComponent<Quad>(entity);
            if (quad.posX <= limitX)
            {
                quad.posX += 5;
            }
            else
            {
                quad.posX = 0;
            }
            
        }
    }

private:
    int limitX = 500;
};


