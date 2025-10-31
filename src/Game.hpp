
#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <memory>
#include <math.h>
#include <typeinfo>

#include <src/ecs/ECS.hpp>
#include <src/systems/SelectionSystem.hpp>
#include <src/systems/RenderingSystem.hpp>
#include <src/systems/DraggingSystem.hpp>
#include <src/systems/ResourceSystem.hpp>
#include <src/systems/CameraSystem.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Quad.hpp>
#include <src/components/Player.hpp>
#include <src/components/Pos2D.hpp>
#include <src/components/Selected.hpp>
#include <src/components/Dragged.hpp>
#include <src/components/ResourceStorage.hpp>
#include <src/components/ResourceGenerator.hpp>
#include <src/io/InputManager.hpp>

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
        
        selectionSystem = coordinator.RegisterSystem<SelectionSystem>();
        {
            Signature<Quad> signature(&coordinator);
            coordinator.SetSystemSignature<SelectionSystem>(signature);
        }

        cameraSystem = coordinator.RegisterSystem<CameraSystem>();
        {
            Signature<Player, Pos2D> signature(&coordinator);
            coordinator.SetSystemSignature<CameraSystem>(signature);
        }


        Entity playerData = coordinator.CreateEntity();
        playerData.Assign<ResourceStorage>(ResourceStorage {0, 0, 0});

        Entity farm = coordinator.CreateEntity();
        farm.Assign<Quad>(Quad {50.0f, 50.0f, 0, 100, 100, 255, 255, 255, 0});
        farm.Assign<Texture>(Texture{"ship.png", 0});
        farm.Assign<ResourceGenerator>(ResourceGenerator {0, 1, 0, 5, playerData.GetId()});

        Entity test = coordinator.CreateEntity();
        test.Assign<Quad>(Quad {150.0f, 150.0f, 0, 100, 100, 255, 255, 255, 45});
        test.Assign<Texture>(Texture{"ship.png", 0});

        Entity player = coordinator.CreateEntity();
        player.Assign<Player>(Player {});
        player.Assign<Pos2D>(Pos2D {0.0f, 0.0f, 0.0f});

        coordinator.RegisterResource<std::vector<Action>>();

        // pugi::xml_document doc;
        // doc.append_child(pugi::node_declaration);

        // auto root = doc.append_child("root");
        // coordinator.ArchiveEntity(root, farm.GetId());
        // doc.save_file("test.xml", PUGIXML_TEXT("  "));

        shared_ptr<Window> testWindow = make_shared<TestWindow>(playerData.GetComponent<ResourceStorage>(), coordinator.GetResource<std::vector<Action>>());
        renderingSystem->AddWindow(testWindow);
    }

    void Start()
    {
        running = true;

        while(running)
        {
            std::vector<Action> actions = inputManager.Update();
            for (const auto action : actions)
            {
                if (action.type == Exit)
                {
                    exit(0);
                }
            }
            coordinator.SetResource<std::vector<Action>>(actions);

            InputState inputState = inputManager.GetInputState();
            CursorPos cursorPos { {inputState.mouseX, inputState.mouseY} };
            coordinator.SetResource<CursorPos>(cursorPos);

            selectionSystem->Update();
            cameraSystem->Update();
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

    shared_ptr<RenderingSystem> renderingSystem;
    shared_ptr<SelectionSystem> selectionSystem;
    shared_ptr<ResourceSystem> resourceSystem;
    shared_ptr<DraggingSystem> draggingSystem;
    shared_ptr<CameraSystem> cameraSystem;

    InputManager inputManager;
};
