
#pragma once

#include <external/cereal/cereal.hpp>

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

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            CEREAL_NVP(posX), 
            CEREAL_NVP(posY), 
            CEREAL_NVP(posZ), 
            CEREAL_NVP(sizeX), 
            CEREAL_NVP(sizeY), 
            CEREAL_NVP(r), 
            CEREAL_NVP(g), 
            CEREAL_NVP(b), 
            CEREAL_NVP(rot)
        );
    }
};
