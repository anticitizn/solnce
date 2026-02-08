
#pragma once

#include <src/utils/macros.hpp>
#include <external/glm/glm.hpp>

// Cartesian transform
struct Transform
{
    glm::dvec2 position;
    glm::dvec2 velocity;
    float rotation;
    
    void archive(pugi::xml_node& root)
    {
        return;
    }
};
