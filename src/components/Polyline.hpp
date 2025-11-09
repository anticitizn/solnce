
#pragma once

#include <vector>

#include <external/glm/glm.hpp>

#include <src/utils/macros.hpp>

struct PolylineAttribute
{
    glm::vec4 color;
    float width;
    float fade;
    float pad1, pad2;
};

struct Polyline
{
    std::vector<glm::vec2> segments;
    PolylineAttribute attribute;

    void archive(pugi::xml_node& root)
    {
        return;
    }
};
