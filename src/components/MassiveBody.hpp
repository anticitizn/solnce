
#pragma once

#include <external/glm/glm.hpp>

#include <src/utils/macros.hpp>

struct MassiveBody
{
    // kg
    double mass;
    
    void archive(pugi::xml_node& root)
    {
        return;
    }
};
