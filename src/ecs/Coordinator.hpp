#pragma once

#include "Coordinator.inc.hpp"
#include "Entity.inc.hpp"
#include "Signature.inc.hpp"

#include "Utils.hpp"
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"
#include "ResourceManager.hpp"

Coordinator::Coordinator()
{
    componentManager = std::make_unique<ComponentManager>();
    entityManager = std::make_unique<EntityManager>();
    systemManager = std::make_unique<SystemManager>();
    resourceManager = std::make_unique<ResourceManager>();
}

Entity Coordinator::CreateEntity()
{
    uint32_t entity_id = entityManager->CreateEntity();
    return Entity(this, entity_id);
}

void Coordinator::DestroyEntity(uint32_t entity)
{
    entityManager->DestroyEntity(entity);
    componentManager->EntityDestroyed(entity);
    systemManager->EntityDestroyed(entity);
}

template <typename T>
void Coordinator::AddComponent(uint32_t entity, T component)
{
    componentManager->AddComponent<T>(entity, component);

    auto signature = entityManager->GetSignature(entity);
    signature.set(componentManager->GetComponentType<T>(), true);
    entityManager->SetSignature(entity, signature);

    systemManager->EntitySignatureChanged(entity, signature);
}

template <typename T>
void Coordinator::RemoveComponent(uint32_t entity)
{
    componentManager->RemoveComponent<T>(entity);

    auto signature = entityManager->GetSignature(entity);
    signature.set(componentManager->GetComponentType<T>(), false);
    entityManager->SetSignature(entity, signature);

    systemManager->EntitySignatureChanged(entity, signature);
}

template<typename T>
bool Coordinator::HasComponent(uint32_t entity)
{
    auto signature = entityManager->GetSignature(entity);
    return signature.test(componentManager->GetComponentType<T>());
}


template <typename T>
T& Coordinator::GetComponent(uint32_t entity)
{
    return componentManager->GetComponent<T>(entity);
}

template <typename T>
void Coordinator::RegisterComponent()
{
    componentManager->RegisterComponent<T>();
}

template <typename T>
uint32_t Coordinator::GetComponentType()
{
    return componentManager->GetComponentType<T>();
}

template <typename T>
std::shared_ptr<T> Coordinator::RegisterSystem()
{
    return systemManager->RegisterSystem<T>();
}

template <typename T, typename... Args>
void Coordinator::SetSystemSignature(Signature<Args...> signature)
{
    systemManager->SetSignature<T>(signature);
}

int Coordinator::GetEntitiesCount()
{
    return entityManager->GetEntitiesCount();
}

template<typename T, typename... Args>
void Coordinator::RegisterResource(Args&&... args)
{
    resourceManager->Register<T>(std::forward<Args>(args)...);
}

template<typename T>
T& Coordinator::GetResource()
{
    return resourceManager->Get<T>();
}

template<typename T>
void Coordinator::SetResource(const T& value)
{
    resourceManager->Set<T>(value);
}

void Coordinator::ArchiveEntity(pugi::xml_node& root, uint32_t entity)
{
    pugi::xml_node entityNode = root.append_child("entity");
    componentManager->ArchiveEntity(entityNode, entity);
}
