
#pragma once

#include <array>
#include <iostream>
#include <unordered_map>
#include <cassert>

#include "Utils.hpp"

#include <external/pugixml/pugixml.hpp>

using namespace std;

class IComponentContainer
{
public:
    virtual void EntityDestroyed(const Entity entity) = 0;
    virtual void ArchiveEntity(pugi::xml_node& root, const Entity entity) = 0;
};

template <class T>
class ComponentContainer : public IComponentContainer
{
public:
    void InsertData(const Entity entity, const T component)
    {
        componentArray[dataSize] = component;
        entityIndexMap.insert({entity, dataSize});
        indexEntityMap.insert({dataSize, entity});

        dataSize++;
    }

    void RemoveData(const Entity entity)
    {
		assert(entityIndexMap.find(entity) != entityIndexMap.end() && "Removing non-existent component.");

		// Copy element at end into deleted element's place to maintain density
		size_t indexOfRemovedEntity = entityIndexMap[entity];
		size_t indexOfLastElement = dataSize - 1;
		componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement];

		// Update map to point to moved spot
		Entity entityOfLastElement = indexEntityMap[indexOfLastElement];
		entityIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		indexEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		entityIndexMap.erase(entity);
		indexEntityMap.erase(indexOfLastElement);

		--dataSize;
    }

    T& GetData(const Entity entity) 
    {
        assert(entityIndexMap.find(entity) != entityIndexMap.end() && "Retrieving non-existent component.");

        return componentArray[entityIndexMap[entity]];
    }

    void EntityDestroyed(const Entity entity) override 
    {
        if (entityIndexMap.find(entity) != entityIndexMap.end())
		{
			RemoveData(entity);
		}
    }

    void ArchiveEntity(pugi::xml_node& root, const Entity entity) override
    {
        if (entityIndexMap.find(entity) != entityIndexMap.end())
        {
            GetData(entity).archive(root);
        }
    }

private:
    array<T, MAX_ENTITIES> componentArray;
    unordered_map<Entity, int> entityIndexMap;
    unordered_map<int, Entity> indexEntityMap;
    int dataSize = 0;
};