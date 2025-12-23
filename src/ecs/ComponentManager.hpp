
#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

#include "Utils.hpp"
#include "ComponentContainer.hpp"

#include <external/pugixml/pugixml.hpp>

using namespace std;

class ComponentManager
{
public:
    template <typename T>
    void RegisterComponent()
    {
        std::type_index typeIdx(typeid(T));

        // Avoid duplicate registrations
        if (componentTypes.contains(typeIdx))
        {
            return;
        }
        
        componentTypes.insert({typeIdx, nextComponentType});
        typeComponents.insert({nextComponentType, typeIdx});
        componentContainers.insert({typeIdx, make_shared<ComponentContainer<T>>()});

        nextComponentType++;
    }

    template <typename T>
    void AddComponent(EntityID entity, T component)
    {
        GetComponentContainer<T>()->InsertData(entity, component);
    }

    template <typename T>
    void RemoveComponent(EntityID entity)
    {
        GetComponentContainer<T>()->RemoveData(entity);
    }

    template <typename T>
    T& GetComponent(EntityID entity)
    {
        return GetComponentContainer<T>()->GetData(entity);
    }

    template <typename T>
    ComponentType GetComponentType()
    {
        EnsureComponentRegistered<T>();

        std::type_index typeIdx(typeid(T));
        return componentTypes[typeIdx];
    }

    void EntityDestroyed(EntityID entity)
    {
        for (auto const& pair : componentContainers)
        {
            auto const& component = pair.second;
            component->EntityDestroyed(entity);
        }
    }

    void ArchiveEntity(pugi::xml_node& root, EntityID entity)
    {
        for (auto const& pair : componentContainers)
        {
            auto const& component = pair.second;
            component->ArchiveEntity(root, entity);
        }
    }

private:
    unordered_map<std::type_index, ComponentType> componentTypes {};
    unordered_map<ComponentType, std::type_index> typeComponents {};
    unordered_map<std::type_index, shared_ptr<IComponentContainer>> componentContainers {};
    ComponentType nextComponentType = 0;

    template <typename T>
    void EnsureComponentRegistered()
    {
        // Runs once per (ComponentManager instance, T) combination
        static const int once = (RegisterComponent<T>(), 0);
        (void)once;
    }

    // Get the ComponentContainer of type T
    template<typename T>
    shared_ptr<ComponentContainer<T>> GetComponentContainer()
    {
        EnsureComponentRegistered<T>();

        std::type_index typeIdx(typeid(T));
        return static_pointer_cast<ComponentContainer<T>>(componentContainers[typeIdx]);
    }

};
