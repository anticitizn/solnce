
#pragma once

#include <src/utils/macros.hpp>
#include <src/ecs/Utils.hpp>

struct ResourceGenerator
{
    float ore;
    float food;
    float alloys;
    int tickrate;
    Entity resourceStorage;

    void archive(pugi::xml_node& root)
    {
        pugi::xml_node arch = root.append_child("ResourceGenerator");

        ARCHIVE_VAR(arch, ore);
        ARCHIVE_VAR(arch, food);
        ARCHIVE_VAR(arch, alloys);
        ARCHIVE_VAR(arch, tickrate);
        ARCHIVE_VAR(arch, resourceStorage);
    }
};
