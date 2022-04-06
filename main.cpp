
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <memory>

#include <src/ecs/ECS.hpp>
#include <src/systems/RenderingSystem.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Position.hpp>
#include <src/components/Size.hpp>

using namespace std;

extern Coordinator coordinator;

int main(int argc, char *argv[]) 
{
    coordinator.RegisterComponent<Texture>();
    coordinator.RegisterComponent<Position>();
    coordinator.RegisterComponent<Size>();

    shared_ptr<RenderingSystem> renderingSystem = coordinator.RegisterSystem<RenderingSystem>();
    
    {
        Signature signature;
        signature.set(coordinator.GetComponentType<Texture>());
        signature.set(coordinator.GetComponentType<Position>());
        signature.set(coordinator.GetComponentType<Size>());
        coordinator.SetSystemSignature<RenderingSystem>(signature);
    }

    renderingSystem->Init("", "src/shaders/");

    cout << coordinator.GetEntitiesCount() << endl;
}