
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
        archive(posX, posY, posZ, sizeX, sizeY, r, g, b, rot);
    }
};
