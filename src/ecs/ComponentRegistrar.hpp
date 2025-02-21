#pragma once

#include <src/ecs/ECS.hpp>

template <typename T>
struct ComponentRegistrar
{
private:
    static bool RegisterComponent()
    {
        coordinator.RegisterComponent<T>();
        return true;
    }

    static inline bool registered = RegisterComponent();
};
