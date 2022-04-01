
#pragma once

#include <array>
#include <iostream>
#include <unordered_map>
#include "Utils.hpp"

using namespace std;

class IComponentContainer
{
public:
    virtual void EntityDestroyed(const EntityID entity) = 0;
};

template <class T>
class ComponentContainer : public IComponentContainer
{
public:
    void InsertData(const EntityID entity)
    {
        T component;

        componentArray[dataSize] = component;
        entityIndexMap.insert({entity, component});
        indexEntityMap.insert({component, entity});

        dataSize++;
    }

    void InsertData(const EntityID entity, const T component)
    {
        componentArray[dataSize] = component;
        entityIndexMap.insert({entity, component});
        indexEntityMap.insert({component, entity});

        dataSize++;
    }

    void RemoveData(const EntityID entity)
    {
        // Removes the component data of the entity and moves last entry of array 
        // into the gap to keep the array packed and cache-friendly

        int removedIndex = entityIndexMap[entity];
        entityIndexMap.erase(entity);
        componentArray[removedIndex] = componentArray[dataSize - 1];

        EntityID replacedEntity = indexEntityMap[dataSize - 1];
        entityIndexMap.at(replacedEntity) = removedIndex;
        indexEntityMap.at(removedIndex) = replacedEntity;

        dataSize--;
    }

    T& GetData(const EntityID entity) 
    {
        int index = entityIndexMap.find(entity);
        return componentArray[index];
    }

    void EntityDestroyed(const EntityID entity) override 
    {
        RemoveData(entity);
    }

private:
    array<T, MAX_ENTITIES> componentArray;
    unordered_map<EntityID, int> entityIndexMap;
    unordered_map<int, EntityID> indexEntityMap;
    int dataSize = 0;
};