
#pragma once

#include <src/utils/math/Vec3.hpp>

#include <src/utils/macros.hpp>

struct Gravity
{
    Vec3 force;

    void archive(pugi::xml_node& root)
    {
        pugi::xml_node arch = root.append_child("Gravity");
        
        ARCHIVE_VAR(arch, filename);
        force.archive(arch);
    }
};
