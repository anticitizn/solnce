
#pragma once

#include <memory>
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
        const char* typeName = typeid(T).name();
        componentTypes.insert({typeName, nextComponentType});
        typeComponents.insert({nextComponentType, typeName});
        componentContainers.insert({typeName, make_shared<ComponentContainer<T>>()});

        nextComponentType++;
    }

    template <typename T>
    void AddComponent(Entity entity, T component)
    {
        GetComponentContainer<T>()->InsertData(entity, component);
    }

    template <typename T>
    void RemoveComponent(Entity entity)
    {
        GetComponentContainer<T>()->RemoveData(entity);
    }

    template <typename T>
    T& GetComponent(Entity entity)
    {
        return GetComponentContainer<T>()->GetData(entity);
    }

    template <typename T>
    ComponentType GetComponentType()
    {
        const char* typeName = typeid(T).name();
        return componentTypes[typeName];
    }

    void EntityDestroyed(Entity entity)
    {
        for (auto const& pair : componentContainers)
        {
            auto const& component = pair.second;
            component->EntityDestroyed(entity);
        }
    }

    void ArchiveEntity(pugi::xml_node& root, Entity entity)
    {
        for (auto const& pair : componentContainers)
        {
            auto const& component = pair.second;
            component->ArchiveEntity(root, entity);
        }
    }

private:
    unordered_map<const char*, ComponentType> componentTypes {};
    unordered_map<ComponentType, const char*> typeComponents {};
    unordered_map<const char *, shared_ptr<IComponentContainer>> componentContainers {};
    ComponentType nextComponentType = 0;

    // Get the ComponentContainer of type T
    template<typename T>
    shared_ptr<ComponentContainer<T>> GetComponentContainer()
    {
        const char* typeName = typeid(T).name();

        return static_pointer_cast<ComponentContainer<T>>(componentContainers[typeName]);
    }
};
