
#pragma once

#include <external/cereal/cereal.hpp>

#include <src/Bar.hpp>

template <class T>
class Foo
{
public:
    T t;
    int a = 10;

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(a), CEREAL_NVP(t)); 
    }
};
