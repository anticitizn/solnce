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
        Signature<Texture, Quad> signature(&coordinator);
        coordinator.SetSystemSignature<RenderingSystem>(signature);
    }

    renderingSystem->Init("assets/", "src/shaders/");

    InputManager inputManager;

    shared_ptr<ResourceSystem> resourceSystem = coordinator.RegisterSystem<ResourceSystem>();
    {
        Signature<ResourceGenerator> signature(&coordinator);
        coordinator.SetSystemSignature<ResourceSystem>(signature);
    }

    shared_ptr<MovingSystem> movingSystem = coordinator.RegisterSystem<MovingSystem>();
    {
        Signature<Quad, Dragged> signature(&coordinator);
        coordinator.SetSystemSignature<MovingSystem>(signature);
    }
    movingSystem->Init(inputManager.GetEvents());
    
    shared_ptr<InputSystem> inputSystem = coordinator.RegisterSystem<InputSystem>();
    {
        Signature<Quad> signature(&coordinator);
        coordinator.SetSystemSignature<InputSystem>(signature);
    }
    inputSystem->Init(inputManager.GetEvents());

    Entity playerData = coordinator.CreateEntity();
    playerData.Assign<ResourceStorage>(ResourceStorage {0, 0, 0});

    Entity farm = coordinator.CreateEntity();
    farm.Assign<Quad>(Quad {50.0f, 50.0f, 0, 100, 100, 255, 255, 255, 0});
    farm.Assign<Texture>(Texture{"wall.jpg", 0});
    farm.Assign<ResourceGenerator>(ResourceGenerator {0, 1, 0, 5, playerData.GetId()});

    Entity test = coordinator.CreateEntity();
    test.Assign<Quad>(Quad {150.0f, 150.0f, 0, 100, 100, 255, 255, 255, 45});
    test.Assign<Texture>(Texture{"wall.jpg", 0});

    pugi::xml_document doc;
    doc.append_child(pugi::node_declaration);

    auto root = doc.append_child("root");
    coordinator.ArchiveEntity(root, farm.GetId());
    doc.save_file("test.xml", PUGIXML_TEXT("  "));

    shared_ptr<Window> testWindow = make_shared<TestWindow>(playerData.GetComponent<ResourceStorage>(), inputManager.GetMouseState());
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