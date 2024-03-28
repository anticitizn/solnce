
#pragma once

#include <string>
#include <iostream>

#include <src/components/ResourceGenerator.hpp>
#include <src/components/ResourceStorage.hpp>

#include <src/ecs/System.hpp>

using namespace std;

extern Coordinator coordinator;

class ResourceSystem : public System
{
public:
    void Update()
    {
        for (const auto& entity : entities)
        {
            auto& resourceGenerator = coordinator.GetComponent<ResourceGenerator>(entity);
            auto& resourceStorage = coordinator.GetComponent<ResourceStorage>(resourceGenerator.resourceStorage);

            if (tick % resourceGenerator.tickrate == 0)
            {
                resourceStorage.ore += resourceGenerator.ore;
                resourceStorage.food += resourceGenerator.food;
                resourceStorage.alloys += resourceGenerator.alloys;
            }

            tick = (tick + 1) % 20;
        }
    }

private:
    int tick = 0;
};


