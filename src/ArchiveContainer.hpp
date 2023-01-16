
#pragma once

#include <external/cereal/cereal.hpp>

#include <src/ecs/Utils.hpp>

template <class T>
class ArchiveContainer
{
public:
    Entity entityId;
    T component;

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(CEREAL_NVP(entityId), CEREAL_NVP(component));
    }
};
