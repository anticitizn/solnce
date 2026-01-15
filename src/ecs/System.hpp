
#pragma once

#include "Signature.hpp"

using namespace std;

class System
{
public:

    virtual Signature InitialSignature(Coordinator& coordinator) = 0;

    set<EntityID> entities;
    // All entities that match the system's signature

    set<EntityID> newEntities; 
    // Entities that have been newly added to the system
    // Useful if heap memory has to be assigned or otherwise data needs to be initialized by the system
    // before an entity is ready to be used
    // Cleared at the beginning of each update frame

    set<EntityID> removedEntities; 
    // Entities that have either been destroyed or whose signature no longer matches that of the system
    // Useful if memory from the heap has to be freed
    // Cleared at the beginning of each update frame
};
