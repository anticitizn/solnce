
#pragma once

#include <string>
#include <external/cereal/cereal.hpp>

struct Texture
{
    string filename;
    unsigned int id = 0;

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            CEREAL_NVP(filename), 
            CEREAL_NVP(id)
        );
    }
};
