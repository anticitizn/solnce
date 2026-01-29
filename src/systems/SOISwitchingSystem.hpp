
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

            
            if (orbitComponent.r > 1)
            {
                
            }
        }
    }

private:

    static double Wrap2Pi(double x)
    {
        x = std::fmod(x, 2.0 * M_PI);
        if (x < 0)
        {
            x += 2.0 * M_PI;
        } 

        return x;
    }

};
