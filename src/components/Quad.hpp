
#pragma once

#include <src/utils/macros.hpp>

struct Quad
{
    float sizeX;
    float sizeY;
    float r;
    float g;
    float b;

    void archive(pugi::xml_node& root)
    {
        pugi::xml_node arch = root.append_child("Quad");

        ARCHIVE_VAR(arch, sizeX);
        ARCHIVE_VAR(arch, sizeY);
        ARCHIVE_VAR(arch, r);
        ARCHIVE_VAR(arch, g);
        ARCHIVE_VAR(arch, b);
    }
};
