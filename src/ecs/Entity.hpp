
#pragma once

#include "Utils.hpp"
#include "ECS.hpp"

class Entity
{
public:
    Entity()
    {
        id = ecs::coordinator.CreateEntity();
    }

    ~Entity()
    {
        ecs::coordinator.DestroyEntity(id);
    }

    template <typename T>
    void AddComponent(T component)
    {
        ecs::coordinator.AddComponent<T>(id, component);
    }

    template <typename T>
    void RemoveComponent()
    {
        ecs::coordinator.RemoveComponent<T>(id);
    }

    template <typename T>
    T& GetComponent()
    {
        return ecs::coordinator.GetComponent<T>(id);
    }

private:
    EntityID id;
};