
#pragma once

#include <src/utils/macros.hpp>

struct Quad
{
    float posX;
    float posY;
    float posZ;
    float sizeX;
    float sizeY;
    float r;
    float g;
    float b;
    float rot;

    void archive(pugi::xml_node& root)
    {
        pugi::xml_node quad = root.append_child("quad");

        ARCHIVE_VAR(quad, posX);
        ARCHIVE_VAR(quad, posY);
        ARCHIVE_VAR(quad, posZ);
        ARCHIVE_VAR(quad, sizeX);
        ARCHIVE_VAR(quad, sizeY);
        ARCHIVE_VAR(quad, r);
        ARCHIVE_VAR(quad, g);
        ARCHIVE_VAR(quad, b);
        ARCHIVE_VAR(quad, rot);
    }
};
