#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <memory>
#include <math.h>
#include <typeinfo>

#include <src/ecs/ECS.hpp>
#include <src/systems/RenderingSystem.hpp>
#include <src/systems/MovingSystem.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Quad.hpp>

#include <src/ui/TestWindow.hpp>

#include <external/pugixml/pugixml.hpp>

using namespace std;

extern Coordinator coordinator;

int WinMain(int argc, char *argv[])
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

    shared_ptr<MovingSystem> movingSystem = coordinator.RegisterSystem<MovingSystem>();

    {
        Signature signature;
        signature.set(coordinator.GetComponentType<Quad>());
        coordinator.SetSystemSignature<MovingSystem>(signature); 
    }
    
    Entity testEntity;
    for (int i = 0; i < 10; i++)
    {
        Entity entity = coordinator.CreateEntity();
        testEntity = entity;
        coordinator.AddComponent<Quad>(entity, Quad {50.0f*i, 50.0f*i, 0, 50, 50, 255, 255, 255, 0});
        coordinator.AddComponent<Texture>(entity, Texture{"", 0});
    }

    pugi::xml_document doc;
    doc.append_child(pugi::node_declaration);

    auto root = doc.append_child("root");
    coordinator.ArchiveEntity(root, testEntity);
    doc.save_file("test.xml", PUGIXML_TEXT("  "));

    shared_ptr<Window> testWindow = make_shared<TestWindow>(coordinator.GetComponent<Quad>(testEntity));
    renderingSystem->AddWindow(testWindow);

    while(true)
    {
        movingSystem->Update();
        renderingSystem->Render();
    }
    
    cout << coordinator.GetEntitiesCount() << endl;
    
}