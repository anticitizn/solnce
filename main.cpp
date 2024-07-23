#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <memory>
#include <math.h>
#include <typeinfo>

#include <src/ecs/ECS.hpp>
#include <src/systems/InputSystem.hpp>
#include <src/systems/RenderingSystem.hpp>
#include <src/systems/MovingSystem.hpp>
#include <src/systems/ResourceSystem.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Quad.hpp>
#include <src/components/Selected.hpp>
#include <src/components/Dragged.hpp>
#include <src/components/ResourceStorage.hpp>
#include <src/components/ResourceGenerator.hpp>
#include <src/InputManager.hpp>

#include <src/ui/TestWindow.hpp>

#include <external/pugixml/pugixml.hpp>

using namespace std;

extern Coordinator coordinator;

int main(int argc, char *argv[])
{
    coordinator.RegisterComponent<Texture>();
    coordinator.RegisterComponent<Quad>();
    coordinator.RegisterComponent<ResourceStorage>();
    coordinator.RegisterComponent<ResourceGenerator>();
    coordinator.RegisterComponent<Selected>();
    coordinator.RegisterComponent<Dragged>();

    shared_ptr<RenderingSystem> renderingSystem = coordinator.RegisterSystem<RenderingSystem>();
    
    {
        Signature signature;
        signature.set(coordinator.GetComponentType<Texture>());
        signature.set(coordinator.GetComponentType<Quad>());
        coordinator.SetSystemSignature<RenderingSystem>(signature);
    }

    renderingSystem->Init("assets/", "src/shaders/");

    InputManager inputManager;

    shared_ptr<ResourceSystem> resourceSystem = coordinator.RegisterSystem<ResourceSystem>();
    {
        Signature signature;
        signature.set(coordinator.GetComponentType<ResourceGenerator>());
        coordinator.SetSystemSignature<ResourceSystem>(signature);
    }

    shared_ptr<MovingSystem> movingSystem = coordinator.RegisterSystem<MovingSystem>();
    {
        Signature signature;
        signature.set(coordinator.GetComponentType<Quad>());
        signature.set(coordinator.GetComponentType<Dragged>());
        coordinator.SetSystemSignature<MovingSystem>(signature);
    }
    movingSystem->Init(inputManager.GetEvents());
    
    shared_ptr<InputSystem> inputSystem = coordinator.RegisterSystem<InputSystem>();
    {
        Signature signature;
        signature.set(coordinator.GetComponentType<Quad>());
        coordinator.SetSystemSignature<InputSystem>(signature);
    }
    inputSystem->Init(inputManager.GetEvents());

    /*
    Entity testEntity;
    for (int i = 0; i < 10; i++)
    {
        Entity entity = coordinator.CreateEntity();
        testEntity = entity;
        coordinator.AddComponent<Quad>(entity, Quad {50.0f*i, 50.0f*i, 0, 50, 50, 255, 255, 255, 0});
        coordinator.AddComponent<Texture>(entity, Texture{"wall.jpg", 0});
    }
    */

    Entity playerData = coordinator.CreateEntity();
    coordinator.AddComponent<ResourceStorage>(playerData, ResourceStorage {0, 0, 0});

    Entity farm = coordinator.CreateEntity();
    coordinator.AddComponent<Quad>(farm, Quad {50.0f, 50.0f, 0, 100, 100, 255, 255, 255, 0});
    coordinator.AddComponent<Texture>(farm, Texture{"wall.jpg", 0});
    coordinator.AddComponent<ResourceGenerator>(farm, ResourceGenerator {0, 1, 0, 5, playerData});

    pugi::xml_document doc;
    doc.append_child(pugi::node_declaration);

    auto root = doc.append_child("root");
    coordinator.ArchiveEntity(root, farm);
    doc.save_file("test.xml", PUGIXML_TEXT("  "));

    shared_ptr<Window> testWindow = make_shared<TestWindow>(coordinator.GetComponent<ResourceStorage>(playerData), inputManager.GetMouseState());
    renderingSystem->AddWindow(testWindow);

    while(true)
    {
        inputManager.Update();
        inputSystem->Update();
        movingSystem->Update();
        renderingSystem->Render();
        resourceSystem->Update();
    }
    
    cout << coordinator.GetEntitiesCount() << endl;
    
}