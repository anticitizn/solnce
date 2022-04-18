
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <memory>

#include <src/ecs/ECS.hpp>
#include <src/systems/RenderingSystem.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Quad.hpp>

using namespace std;

extern Coordinator coordinator;

int main(int argc, char *argv[]) 
{
    coordinator.RegisterComponent<Texture>();
    coordinator.RegisterComponent<Quad>();

    shared_ptr<RenderingSystem> renderingSystem = coordinator.RegisterSystem<RenderingSystem>();
    
    {
        Signature signature;
        signature.set(coordinator.GetComponentType<Texture>());
        signature.set(coordinator.GetComponentType<Quad>());
        coordinator.SetSystemSignature<RenderingSystem>(signature);
    }

    renderingSystem->Init("assets/", "src/shaders/");

    cout << coordinator.GetEntitiesCount() << endl;
}