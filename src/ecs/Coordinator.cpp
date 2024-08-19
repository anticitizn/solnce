
#include "Coordinator.hpp"
#include "Entity.hpp"

Entity Coordinator::CreateEntity()
{
    uint32_t entity_id = entityManager->CreateEntity();
    return Entity(this, entity_id);
}