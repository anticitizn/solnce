#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <memory>
#include <math.h>
#include <typeinfo>

#include <src/ecs/ECS.hpp>
#include <src/systems/RenderingSystem.hpp>
#include <src/systems/SaveSystem.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Quad.hpp>
#include <src/Foo.hpp>
#include <src/Bar.hpp>

#include <fstream>

#include <cereal/archives/xml.hpp>

using namespace std;

extern Coordinator coordinator;

int main(int argc, char *argv[])
{
    Foo<Bar> foo;

    ofstream filestream("test.xml");
    {
        cereal::XMLOutputArchive archive(filestream);
        archive(CEREAL_NVP(foo));
    }

    filestream.flush();
    filestream.close();

    cout << "Archived" << endl;

    coordinator.RegisterComponent<Texture>();
    coordinator.RegisterComponent<Quad>();

    shared_ptr<SaveSystem> saveSystem = coordinator.RegisterSystem<SaveSystem>();

    {
        Signature signature;
        signature.set(coordinator.GetComponentType<Texture>());
        signature.set(coordinator.GetComponentType<Quad>());
        coordinator.SetSystemSignature<SaveSystem>(signature);
    }

    shared_ptr<RenderingSystem> renderingSystem = coordinator.RegisterSystem<RenderingSystem>();
    
    {
        Signature signature;
        signature.set(coordinator.GetComponentType<Texture>());
        signature.set(coordinator.GetComponentType<Quad>());
        coordinator.SetSystemSignature<RenderingSystem>(signature);
    }

    renderingSystem->Init("assets/", "src/shaders/");
    
    for (int i = 0; i < 10; i++)
    {
        Entity entity = coordinator.CreateEntity();
        coordinator.AddComponent<Quad>(entity, Quad {50.0f * i, 50.0f * i, 0, 50, 50, 255, 255, 255, 0});
        coordinator.AddComponent<Texture>(entity, Texture{"", 0});
    }

    while(true)
    {
        renderingSystem->Render();
    }
    
    cout << coordinator.GetEntitiesCount() << endl;
    
}