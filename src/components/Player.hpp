
#pragma once

#include <src/utils/macros.hpp>

struct Player
{
    float speed = 10.0f;
    
    void archive(pugi::xml_node& root)
    {
        return;
    }
};
