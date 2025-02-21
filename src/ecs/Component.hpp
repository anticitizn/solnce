#pragma once

#include "Coordinator.hpp"

template <typename T>
struct ComponentBase
{
private:
    static bool RegisterComponent()
    {
        coordinator.RegisterComponent<T>();
        return true;
    }

    static inline bool registered = RegisterComponent();
};
