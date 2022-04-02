#include <iostream>
#include <string>
#include <memory>

#include <src/ecs/ECS.hpp>
#include <src/systems/RenderingSystem.hpp>
#include <src/components/Texture.hpp>

using namespace std;

extern Coordinator coordinator;

int main(int argc, char *argv[]) 
{
    coordinator.RegisterComponent<Texture>();

    shared_ptr<RenderingSystem> renderingSystem = coordinator.RegisterSystem<RenderingSystem>();
    
    {
        Signature signature;
        signature.set(coordinator.GetComponentType<Texture>());
        coordinator.SetSystemSignature<RenderingSystem>(signature);
    }

    renderingSystem->Init();

    cout << coordinator.GetEntitiesCount() << endl;
}