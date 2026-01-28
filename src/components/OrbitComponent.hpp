
#pragma once

#include <src/ecs/Entity.hpp>

#include <src/utils/macros.hpp>

struct OrbitComponent
{
    uint32_t parentBodyId;
    
    // Initial parameters (used by simulation)
    double rp;  // Periapsis radius
    double e;   // Eccentricity
    double ap;  // Argument of periapsis (how the periapsis is rotated related to the x-axis)
    double ta;  // True anomaly

    // Derived parameters
    double a;   // Semimajor axis
    double r;   // Radius
    
    void archive(pugi::xml_node& root)
    {
        return;
    }
};
