#pragma once

#include <memory>
#include <external/pugixml/pugixml.hpp>
#include "Signature.hpp"

class Entity;

class Coordinator
{
public:
    Coordinator();

    Entity CreateEntity();
    void DestroyEntity(uint32_t entity);

    template <typename T>
    void AddComponent(uint32_t entity, T component);

    template <typename T>
    void RemoveComponent(uint32_t entity);

    template<typename T>
    bool HasComponent(uint32_t entity);

    template <typename T>
    T& GetComponent(uint32_t entity);

    template <typename T>
    void RegisterComponent();

    template <typename T>
    uint32_t GetComponentType();

    template <typename T>
    std::shared_ptr<T> RegisterSystem();

    template <typename T, typename... Args>
    void SetSystemSignature(Signature signature);

    int GetEntitiesCount();

    template<typename T, typename... Args>
    void RegisterResource(Args&&... args);

    template<typename T>
    T& GetResource();

    template<typename T>
    void SetResource(const T& value);

    void ArchiveEntity(pugi::xml_node& root, uint32_t entity);

    template <typename... Args>
    Signature BuildSignature();

private:
    std::unique_ptr<class ComponentManager> componentManager;
    std::unique_ptr<class EntityManager> entityManager;
    std::unique_ptr<class SystemManager> systemManager;
    std::unique_ptr<class ResourceManager> resourceManager;
};
