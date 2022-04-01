
#pragma once

#include "Utils.hpp"
#include "ECS.hpp"

class Entity
{
public:
    Entity()
    {
        id = ecs::entityManager.CreateEntity();
    }

    ~Entity()
    {
        ecs::entityManager.DestroyEntity(id);
        ecs::componentManager.EntityDestroyed(id);
        ecs::systemManager.EntityDestroyed(id);
    }

    template <typename T>
    void AddComponent(T component)
    {
        ecs::componentManager.AddComponent<T>(id, component);

        auto signature = ecs::entityManager.GetSignature(id);
        signature.set(ecs::componentManager.GetComponentType<T>(), true);
        ecs::entityManager.SetSignature(id, signature);

        ecs::systemManager.EntitySignatureChanged(id, signature);
    }

    template <typename T>
    void RemoveComponent()
    {
        ecs::componentManager.RemoveComponent<T>(id);

        auto signature = ecs::entityManager.GetSignature(id);
        signature.set(ecs::componentManager.GetComponentType<T>(), false);
        ecs::entityManager.SetSignature(id, signature);

        ecs::systemManager.EntitySignatureChanged(id, signature);
    }

    template <typename T>
    T& GetComponent()
    {
        return ecs::componentManager.GetComponent<T>(id);
    }

private:
    EntityID id;
};