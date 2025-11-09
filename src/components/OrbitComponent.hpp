
#pragma once

#include <src/ecs/Entity.hpp>

#include <src/utils/macros.hpp>

struct OrbitComponent
{
    uint32_t parentBodyId;
    
    double a;   // Semimajor axis
    double e;   // Eccentricity
    double ap;  // Argument of periapsis (angle between ascending node and periapsis)
    double ta;  // True anomaly

    double E;   // Eccentric anomaly
    double M;   // Mean anomaly
    double r;   // Radius
    
    void archive(pugi::xml_node& root)
    {
        return;
    }
};
