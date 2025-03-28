
#pragma once

#include <src/utils/macros.hpp>
#include <src/utils/math/Vec2.hpp>

struct Player
{
    float speed = 3.0f;
    Vec2 velocity;
    
    void archive(pugi::xml_node& root)
    {
        return;
    }
};
