
#pragma once

#include <vector>

#include <external/glm/glm.hpp>

#include <src/utils/macros.hpp>

struct MassiveBody
{
    double mass; // kg
    double soi;  // m

    std::vector<uint32_t> childrenIds;
    
    void archive(pugi::xml_node& root)
    {
        return;
    }
};
