
#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <memory>
#include <math.h>
#include <typeinfo>

#include <src/ecs/ECS.hpp>
#include <src/systems/InputSystem.hpp>
#include <src/systems/RenderingSystem.hpp>
#include <src/systems/DraggingSystem.hpp>
#include <src/systems/ResourceSystem.hpp>
#include <src/systems/PlayerMovementSystem.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Quad.hpp>
#include <src/components/Player.hpp>
#include <src/components/Pos2D.hpp>
#include <src/components/Selected.hpp>
#include <src/components/Dragged.hpp>
#include <src/components/ResourceStorage.hpp>
#include <src/components/ResourceGenerator.hpp>
#include <src/InputManager.hpp>

#include <src/ui/TestWindow.hpp>

#include <external/pugixml/pugixml.hpp>

using namespace std;

extern Coordinator coordinator;

class Game
{
public:
    Game()
    {
        coordinator.RegisterComponent<Texture>();
        coordinator.RegisterComponent<Pos2D>();
        coordinator.RegisterComponent<Player>();
        coordinator.RegisterComponent<Quad>();
        coordinator.RegisterComponent<ResourceStorage>();
        coordinator.RegisterComponent<ResourceGenerator>();
        coordinator.RegisterComponent<Selected>();
        coordinator.RegisterComponent<Dragged>();

        renderingSystem = coordinator.RegisterSystem<RenderingSystem>();
        {
            Signature<Texture, Quad> signature(&coordinator);
            coordinator.SetSystemSignature<RenderingSystem>(signature);
        }

        renderingSystem->Init("assets/", "src/shaders/");

        resourceSystem = coordinator.RegisterSystem<ResourceSystem>();
        {
            Signature<ResourceGenerator> signature(&coordinator);
            coordinator.SetSystemSignature<ResourceSystem>(signature);
        }

        draggingSystem = coordinator.RegisterSystem<DraggingSystem>();
        {
            Signature<Quad, Dragged> signature(&coordinator);
            coordinator.SetSystemSignature<DraggingSystem>(signature);
        }
        draggingSystem->Init(inputManager.GetEvents());
        
        inputSystem = coordinator.RegisterSystem<InputSystem>();
        {
            Signature<Quad> signature(&coordinator);
            coordinator.SetSystemSignature<InputSystem>(signature);
        }
        inputSystem->Init(inputManager.GetEvents());

        playerMovementSystem = coordinator.RegisterSystem<PlayerMovementSystem>();
        {
            Signature<Player, Pos2D> signature(&coordinator);
            coordinator.SetSystemSignature<PlayerMovementSystem>(signature);
        }
        playerMovementSystem->Init(inputManager.GetEvents());


        Entity playerData = coordinator.CreateEntity();
        playerData.Assign<ResourceStorage>(ResourceStorage {0, 0, 0});

        Entity farm = coordinator.CreateEntity();
        farm.Assign<Quad>(Quad {50.0f, 50.0f, 0, 100, 100, 255, 255, 255, 0});
        farm.Assign<Texture>(Texture{"wall.jpg", 0});
        farm.Assign<ResourceGenerator>(ResourceGenerator {0, 1, 0, 5, playerData.GetId()});

        Entity test = coordinator.CreateEntity();
        test.Assign<Quad>(Quad {150.0f, 150.0f, 0, 100, 100, 255, 255, 255, 45});
        test.Assign<Texture>(Texture{"wall.jpg", 0});

        Entity player = coordinator.CreateEntity();
        player.Assign<Player>(Player {});
        player.Assign<Pos2D>(Pos2D {0.0f, 0.0f, 0.0f});

        pugi::xml_document doc;
        doc.append_child(pugi::node_declaration);

        auto root = doc.append_child("root");
        coordinator.ArchiveEntity(root, farm.GetId());
        doc.save_file("test.xml", PUGIXML_TEXT("  "));

        shared_ptr<Window> testWindow = make_shared<TestWindow>(playerData.GetComponent<ResourceStorage>(), inputManager.GetMouseState());
        renderingSystem->AddWindow(testWindow);
    }

    void Start()
    {
        running = true;

        while(running)
        {
            inputManager.Update();
            inputSystem->Update();
            playerMovementSystem->Update();
            draggingSystem->Update();
            renderingSystem->Render();
            resourceSystem->Update();
        }
    }

    void Stop()
    {
        running = false;
    }

private:
    bool running;

    InputManager inputManager;
    shared_ptr<RenderingSystem> renderingSystem;
    shared_ptr<InputSystem> inputSystem;
    shared_ptr<ResourceSystem> resourceSystem;
    shared_ptr<DraggingSystem> draggingSystem;
    shared_ptr<PlayerMovementSystem> playerMovementSystem;
};
