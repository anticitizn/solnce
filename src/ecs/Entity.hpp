#pragma once

#include "Entity.inc.hpp"
#include "Coordinator.inc.hpp"

Entity::Entity(Coordinator* _coordinator, uint32_t _entity_id)
    : coordinator(_coordinator), entity_id(_entity_id)
{
}

uint32_t Entity::GetId()
{
    return entity_id;
}

void Entity::Destroy()
{
    coordinator->DestroyEntity(entity_id);
}

template <typename T>
void Entity::Assign(T component)
{
    coordinator->AddComponent<T>(entity_id, component);
}

template <typename T>
void Entity::Unassign()
{
    coordinator->RemoveComponent<T>(entity_id);
}

template <typename T>
T& Entity::GetComponent()
{
    return coordinator->GetComponent<T>(entity_id);
}
