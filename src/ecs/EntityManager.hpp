
#pragma once

#include <set>
#include <iostream>
#include <queue>
#include "Utils.hpp"

using namespace std;

class EntityManager
{
public:
    EntityManager() 
    {
        for (int i = 0; i < MAX_ENTITIES; i++) 
        {
            unusedEntities.push(i);
        }
    }

    EntityID CreateEntity() 
    {
        if (livingEntities >= MAX_ENTITIES) 
        {
            cout << "ERROR: Maximum amount of entities reached" << endl;
            // amazing error handling, right?
        }

        EntityID newEntity = unusedEntities.front();
        unusedEntities.pop();
        livingEntities++;

        return newEntity;
    }

    void DestroyEntity(EntityID entity)
    {
        assert(entity < MAX_ENTITIES && "Entity out of range.");

        signatures[entity].reset();
        unusedEntities.push(entity);
        livingEntities--;
    }

    void SetSignature(EntityID entity, std::bitset<MAX_COMPONENTS> signature)
    {
        signatures[entity] = signature;
    }

    std::bitset<MAX_COMPONENTS> GetSignature(EntityID entity)
    {
        assert(entity < MAX_ENTITIES && "Entity out of range.");
        
        return signatures[entity];
    }

    int GetEntitiesCount()
    {
        return livingEntities;
    }


private:
    int livingEntities = 0;
    queue<EntityID> unusedEntities;
    array<std::bitset<MAX_COMPONENTS>, MAX_ENTITIES> signatures;
};
