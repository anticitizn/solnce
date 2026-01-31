
#pragma once

#include <vector>

#include <external/glm/glm.hpp>

#include <src/utils/macros.hpp>

struct MassiveBody
{
    // kg
    double mass;

    std::vector<uint32_t> childrenIds;
    
    void archive(pugi::xml_node& root)
    {
        return;
    }
};
