
#pragma once

#include "Coordinator.hpp"

using namespace std;

class Entity
{
public:
    Entity(Coordinator* _coordinator, uint32_t _entity_id)
    {
        coordinator = _coordinator;
        entity_id = _entity_id;
    }

    uint32_t GetId()
    {
        return entity_id;
    }
    
    void Destroy()
    {
        coordinator->DestroyEntity(entity_id);
    }

    template <typename T>
    void Assign(T component)
    {
        coordinator->AddComponent(entity_id, component);
    }

    template <typename T>
    void Unassign(Entity entity)
    {
        coordinator->RemoveComponent<T>(entity_id);
    }

    template <typename T>
    T& GetComponent()
    {
        return coordinator->GetComponent<T>(entity_id);
    }
private:
    uint32_t entity_id;
    Coordinator* coordinator;
};
