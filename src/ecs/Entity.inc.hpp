#pragma once

#include <cstdint>

class Coordinator;

class Entity
{
public:
    Entity(Coordinator* _coordinator, uint32_t _entity_id);

    uint32_t GetId();
    void Destroy();

    template <typename T>
    void Assign(T component);

    template <typename T>
    void Unassign();

    template <typename T>
    T& GetComponent();

private:
    uint32_t entity_id;
    Coordinator* coordinator;
};
