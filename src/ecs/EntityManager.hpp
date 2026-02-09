
#pragma once

#include <set>
#include <iostream>
#include <queue>
#include "Utils.hpp"

using namespace std;

constexpr uint32_t INVALID_ENTITY = 0;

class EntityManager
{
public:
    EntityManager() 
    {
        for (int i = 1; i < MAX_ENTITIES; i++) 
        {
            unusedEntities.push(i);
        }
    }

    EntityID CreateEntity() 
    {
        assert(!unusedEntities.empty() && "Max amount of entities reached");

        EntityID entity = unusedEntities.front();
        unusedEntities.pop();
        
        aliveIndex[entity] = static_cast<int>(alive.size());
        alive.push_back(entity);

        return entity;
    }

    void DestroyEntity(EntityID entity)
    {
        assert(entity < MAX_ENTITIES && "Entity out of range.");

        signatures[entity].reset();

        int idx = aliveIndex[entity];
        EntityID last = alive.back();

        alive[idx] = last;
        aliveIndex[last] = idx;

        alive.pop_back();

        unusedEntities.push(entity);
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
        return static_cast<int>(alive.size());
    }

    const std::vector<EntityID>& GetLivingEntities() const
    {
        return alive;
    }

private:
    std::array<int, MAX_ENTITIES> aliveIndex{}; // maps entityID to array index in alive
    std::vector<EntityID> alive;                // dense list of all living entities
    queue<EntityID> unusedEntities;
    array<std::bitset<MAX_COMPONENTS>, MAX_ENTITIES> signatures;
};
