#include <iostream>
#include <string>

#include <src/ecs/ECS.hpp>
#include <src/systems/RenderingSystem.hpp>

using namespace std;

extern Coordinator coordinator;

int main(int argc, char *argv[]) 
{
    cout << coordinator.GetEntitiesCount() << endl;
}