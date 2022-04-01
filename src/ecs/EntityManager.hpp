
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
        signatures[entity].reset();
        unusedEntities.push(entity);
        livingEntities--;
    }

    void SetSignature(EntityID entity, Signature signature)
    {
        signatures[entity] = signature;
    }

    Signature GetSignature(EntityID entity)
    {
        return signatures[entity];
    }

    int GetEntitiesCount()
    {
        return livingEntities;
    }


private:
    int livingEntities = 0;
    queue<EntityID> unusedEntities;
    array<Signature, MAX_ENTITIES> signatures;
};
