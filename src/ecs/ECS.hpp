
#pragma once

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"

using namespace std;

namespace ecs
{
    ComponentManager componentManager;
    EntityManager entityManager;
    SystemManager systemManager;
}