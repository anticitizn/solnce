
#pragma once

#include <src/utils/macros.hpp>

struct Pos2D
{
    float posX;
    float posY;
    float posZ;
    float rot;

    void archive(pugi::xml_node& root)
    {
        pugi::xml_node arch = root.append_child("Pos2D");

        ARCHIVE_VAR(arch, posX);
        ARCHIVE_VAR(arch, posY);
        ARCHIVE_VAR(arch, posZ);
        ARCHIVE_VAR(arch, rot);
    }
};
