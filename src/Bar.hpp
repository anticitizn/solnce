
#pragma once

#include <external/cereal/cereal.hpp>

class Bar
{
public:
    int b = 50;

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(b)); 
    }
};
