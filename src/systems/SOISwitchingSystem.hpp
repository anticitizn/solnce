
#pragma once

#include <string>
#include <iostream>
#include <algorithm>

#include <src/components/OrbitComponent.hpp>
#include <src/components/MassiveBody.hpp>
#include <src/components/Transform.hpp>
#include <src/resources/SimulationTime.hpp>

#include <src/ecs/System.hpp>

#define G 6.674e-11

extern Coordinator coordinator;

class SOISwitchingSystem : public System
{
public:
    Signature InitialSignature()
    {
        return coordinator.BuildSignature<OrbitComponent, Transform>();
    }

    void Update()
    {
        for (auto e : entities)
        {
            auto& orbitComponent = coordinator.GetComponent<OrbitComponent>(e);
            auto& parentMassiveBodyComponent = coordinator.GetComponent<MassiveBody>(orbitComponent.parentBodyId);
            auto& parentOrbitComponent = coordinator.GetComponent<OrbitComponent>(orbitComponent.parentBodyId);

            uint32_t newParentEntityId = NULL;

            // Check if the entity's primary body is itself orbiting another body
            if (parentOrbitComponent.parentBodyId != NULL)
            {
                auto& parentParentMassiveOrbitComponent = coordinator.GetComponent<MassiveBody>(parentOrbitComponent.parentBodyId);

                double parentSOI = parentOrbitComponent.a * std::pow(parentMassiveBodyComponent.mass / parentParentMassiveOrbitComponent.mass, 0.4);

                if (orbitComponent.r > parentSOI)
                {
                    newParentEntityId = parentOrbitComponent.parentBodyId;
                }
            }
            else
            {
                // The entity's primary body is the star, which has no primary body of its own
                
            }
        }
    }

private:
    

};
