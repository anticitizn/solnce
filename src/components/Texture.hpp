
#pragma once

#include <string>

#include <src/utils/macros.hpp>

struct Texture
{
    string filename;
    unsigned int id = 0;

    void archive(pugi::xml_node& root)
    {
        pugi::xml_node arch = root.append_child("texture");
        
        ARCHIVE_VAR(quad, filename);
    }
};
