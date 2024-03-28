
#pragma once

#include <src/utils/macros.hpp>

struct ResourceStorage
{
    float ore;
    float food;
    float alloys;

    void archive(pugi::xml_node& root)
    {
        pugi::xml_node arch = root.append_child("ResourceStorage");

        ARCHIVE_VAR(arch, ore);
        ARCHIVE_VAR(arch, food);
        ARCHIVE_VAR(arch, alloys);
    }
};
