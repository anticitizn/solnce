
#pragma once

#include <external/pugixml/pugixml.hpp>

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
        pugi::xml_node quad = root.append_child(VAR_STR(posX));

        archiveVar<float>(quad, posX);
        archiveVar<float>(quad, posY);
        archiveVar<float>(quad, posZ);
        archiveVar<float>(quad, sizeX);
        archiveVar<float>(quad, sizeY);
        archiveVar<float>(quad, r);
        archiveVar<float>(quad, g);
        archiveVar<float>(quad, b);
        archiveVar<float>(quad, rot);
    }

    // Convert to macro to preserve variable name
    template <class T>
    void archiveVar(pugi::xml_node& root, T t)
    {
        pugi::xml_node nodeChild = root.append_child(VAR_STR(t));
        nodeChild.append_child(pugi::node_pcdata).set_value(to_string(t).c_str());
    }
};
