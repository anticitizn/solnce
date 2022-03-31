
#include <array>
#include <iostream>
#include <unordered_map>
#include "Utils.hpp"

using namespace std;

class IComponentContainer
{
public:
    virtual void EntityDestroyed(const Entity entity) = 0;
}

template <class T>
class ComponentContainer : public IComponentContainer
{
public:
    void InsertData(const Entity entity) 
    {
        if (entityComponentMap.find(entity)) {
            cout << "Entity already has this component" << endl;
            return;
        }
        Component component;

        componentArray[dataSize] = component;
        entityComponentMap.insert({entity, component});
        componentEntityMap.insert({component, entity});
        dataSize++;
    }

    void RemoveData(const Entity entity) 
    {
        // Removes the component data of the entity and moves last entry of array 
        // into the gap to keep the array packed and cache-friendly

        int removedIndex = entityIndexMap.find(entity);
        entityIndexMap.erase(entity);
        componentArray[removedIndex] = componentArray[dataSize - 1];

        Entity replacedEntity = indexEntityMap.find(dataSize - 1);
        entityIndexMap.at(replacedEntity) = removedIndex;
        indexEntityMap.at(removedIndex) = replacedEntity;
    }

    T& GetData(const Entity entity) 
    {
        int index = entityIndexMap.find(entity);
        return componentArray[index];
    }

    void EntityDestroyed(const Entity entity) override 
    {
        RemoveData(entity);
    }

private:
    array<T, MAX_ENTITIES> componentArray;
    unordered_map<Entity, int> entityIndexMap;
    unordered_map<int, Entity> indexEntityMap;
    int dataSize = 0;
}