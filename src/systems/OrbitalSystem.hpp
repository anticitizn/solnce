
#pragma once

#include <src/ecs/System.hpp>

using namespace std;

class OrbitalSystem : public System
{
public:
    void Update()
    {
        for (const auto& entity : entities)
        {
            auto& quad = coordinator.GetComponent<Quad>(entity);
            if (quad.posX <= limitX)
            {
                quad.posX += 1;
            }
            else
            {
                quad.posX = 0;
            }
            
        }
    }

private:


}