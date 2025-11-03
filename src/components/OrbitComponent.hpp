
#pragma once

#include <src/utils/macros.hpp>
#include <src/utils/math/Vec2.hpp>

struct OrbitComponent
{
    // Semimajor axis
    double a;

    // Eccentricity
    double e;

    // Argument of periapsis (angle between ascending node and periapsis)
    double ap;

    // True anomaly
    double ta;
    
    void archive(pugi::xml_node& root)
    {
        return;
    }
};
