
#pragma once

#include <src/ecs/Entity.hpp>

#include <src/utils/macros.hpp>

struct OrbitComponent
{
    uint32_t parentBodyId;
    
    // Core parameters (needed to fully define an orbit)
    double rp;          // Periapsis radius
    double e;           // Eccentricity
    double ap;          // Argument of periapsis (how the periapsis is rotated related to the x-axis)
    double ta;          // True anomaly (nu)
    double t_periapsis; // The time when periapsis was last passed (tau)

    // Cached parameters recomputed upon SOI change or impulse events
    double a;   // Semimajor axis
    double mu;  // G * parentMass
    double p;   // perifocal point p = rp * (1 + e) 

    bool dirty = true; // When this flag is true, cached parameters must be recomputed
    
    // Parameters derived every update step
    double r;   // Radius

    void archive(pugi::xml_node& root)
    {
        return;
    }
};
