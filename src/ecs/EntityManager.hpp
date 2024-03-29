
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

    Entity CreateEntity() 
    {
        if (livingEntities >= MAX_ENTITIES) 
        {
            cout << "ERROR: Maximum amount of entities reached" << endl;
            // amazing error handling, right?
        }

        Entity newEntity = unusedEntities.front();
        unusedEntities.pop();
        livingEntities++;

        return newEntity;
    }

    void DestroyEntity(Entity entity)
    {
        assert(entity < MAX_ENTITIES && "Entity out of range.");

        signatures[entity].reset();
        unusedEntities.push(entity);
        livingEntities--;
    }

    void SetSignature(Entity entity, Signature signature)
    {
        signatures[entity] = signature;
    }

    Signature GetSignature(Entity entity)
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
    queue<Entity> unusedEntities;
    array<Signature, MAX_ENTITIES> signatures;
};
