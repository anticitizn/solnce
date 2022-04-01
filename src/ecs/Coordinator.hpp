#pragma once

#include <memory>
#include "Utils.hpp"
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"

class Coordinator
{
public:
    Coordinator()
    {
        componentManager = make_unique<ComponentManager>();
        entityManager = make_unique<EntityManager>();
        systemManager = make_unique<SystemManager>();
    }

    EntityID CreateEntity()
    {
        return entityManager->CreateEntity();
    }

    void DestroyEntity(EntityID entity)
    {
        entityManager->DestroyEntity(entity);
        componentManager->EntityDestroyed(entity);
        systemManager->EntityDestroyed(entity);
    }

    template <typename T>
    void AddComponent(EntityID entity, T component)
    {
        componentManager->AddComponent<T>(entity, component);

        auto signature = entityManager->GetSignature(entity);
        signature.set(componentManager->GetComponentType<T>(), true);
        entityManager->SetSignature(entity, signature);

        systemManager->EntitySignatureChanged(entity, signature);
    }

    template <typename T>
    void RemoveComponent(EntityID entity)
    {
        componentManager->RemoveComponent<T>(entity);

        auto signature = entityManager->GetSignature(entity);
        signature.set(componentManager->GetComponentType<T>(), false);
        entityManager->SetSignature(entity, signature);

        systemManager->EntitySignatureChanged(entity, signature);
    }

    template <typename T>
    T& GetComponent(EntityID entity)
    {
        return componentManager->GetComponent<T>(entity);
    }

    template <typename T>
    void RegisterComponent()
    {
        componentManager->RegisterComponent<T>();
    }

    template <typename T>
    ComponentType GetComponentType()
    {
        return componentManager->GetComponentType<T>();
    }

    template <typename T>
    shared_ptr<T> RegisterSystem()
    {
        return systemManager->RegisterSystem<T>();
    }

    template <typename T>
    void SetSystemSignature(Signature signature)
    {
        systemManager->SetSignature<T>(signature);
    }

    int GetEntitiesCount()
    {
        return entityManager->GetEntitiesCount();
    }

private:
    unique_ptr<ComponentManager> componentManager;
    unique_ptr<EntityManager> entityManager;
    unique_ptr<SystemManager> systemManager;
};
