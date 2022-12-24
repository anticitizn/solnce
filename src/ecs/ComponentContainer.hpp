
#pragma once

#include <array>
#include <iostream>
#include <unordered_map>
#include <cassert>

#include <external/cereal/cereal.hpp>
#include <cereal/archives/xml.hpp>
#include <external/cereal/types/set.hpp>

#include "Utils.hpp"

using namespace std;

class IComponentContainer
{
public:
    virtual void EntityDestroyed(const Entity entity) = 0;
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

        ofstream filestream("entity_test.xml");
        {
            cereal::XMLOutputArchive archive(filestream);
            archive(componentArray[entityIndexMap[entity]]);
        }

        return componentArray[entityIndexMap[entity]];
    }

    void EntityDestroyed(const Entity entity) override 
    {
        if (entityIndexMap.find(entity) != entityIndexMap.end())
		{
			RemoveData(entity);
		}
    }

private:
    array<T, MAX_ENTITIES> componentArray;
    unordered_map<Entity, int> entityIndexMap;
    unordered_map<int, Entity> indexEntityMap;
    int dataSize = 0;
};