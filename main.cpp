
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <memory>
#include <math.h>
#include <typeinfo>

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
    
    Entity testEntity = coordinator.CreateEntity();
    coordinator.AddComponent<Quad>(testEntity, Quad {200, 200, 0, 100, 100, 255, 255, 255, 45});
    coordinator.AddComponent<Texture>(testEntity, Texture{"", 0});
    
    while(true)
    {
        renderingSystem->Render();
    }
    
    cout << coordinator.GetEntitiesCount() << endl;
    
}