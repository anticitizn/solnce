
#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <memory>
#include <math.h>
#include <typeinfo>

#include <src/ecs/ECS.hpp>
#include <src/systems/SelectionSystem.hpp>
#include <src/systems/RenderSystem.hpp>
#include <src/systems/DraggingSystem.hpp>
#include <src/systems/ResourceSystem.hpp>
#include <src/systems/CameraSystem.hpp>
#include <src/systems/KeplerOrbitSystem.hpp>
#include <src/systems/OrbitPathSystem.hpp>
#include <src/resources/SimulationTime.hpp>
#include <src/io/InputManager.hpp>

#include <src/ui/TestWindow.hpp>
#include <src/ui/OrbitalDebugWindow.hpp>
#include <src/ui/OrbitalBodyCreationWindow.hpp>

#include <external/pugixml/pugixml.hpp>

using namespace std;

extern Coordinator coordinator;

class Game
{
public:
    Game()
    {
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

        orbitPathSystem = coordinator.RegisterSystem<OrbitPathSystem>();
        {
            Signature<OrbitComponent, Polyline, Transform> sig(&coordinator);
            coordinator.SetSystemSignature<OrbitPathSystem>(sig);
        }

        cameraSystem = coordinator.RegisterSystem<CameraSystem>();
        {
            Signature<Pos2D> signature(&coordinator);
            coordinator.SetSystemSignature<CameraSystem>(signature);
        }

        orbitSystem = coordinator.RegisterSystem<KeplerOrbitSystem>();
        {
            Signature<OrbitComponent, Transform> signature(&coordinator);
            coordinator.SetSystemSignature<KeplerOrbitSystem>(signature);
        }

        renderSystem = coordinator.RegisterSystem<RenderSystem>();        
        renderSystem->Init();

        coordinator.RegisterResource<std::vector<Action>>();

        Entity playerData = coordinator.CreateEntity();
        playerData.Assign<ResourceStorage>(ResourceStorage {0, 0, 0});

        orbitSystem->AddTestBodies();

        // Entity farm = coordinator.CreateEntity();
        // farm.Assign<Quad>(Quad {100, 100, 255, 255, 255});
        // farm.Assign<Transform>(Transform {{50.0f, 50.0f, 0.0f}, {0, 0}, 0});
        // farm.Assign<Texture>(Texture{"ship.png", 0});
        // farm.Assign<ResourceGenerator>(ResourceGenerator {0, 1, 0, 5, playerData.GetId()});

        // Entity test = coordinator.CreateEntity();
        // test.Assign<Quad>(Quad {100, 100, 255, 255, 255});
        // test.Assign<Transform>(Transform {{150.0f, 150.0f, 0}, {0, 0}, 45});
        // test.Assign<Texture>(Texture{"missing-texture.png", 0});

        // {
        //     Polyline line = { {{0, 0}, {100, 0}, {200, 0}, {200, 100}, {100, 150}}, { {1, 0, 0, 1},  1.0f, 1.0f, 0, 0 }};
        //     Entity lineEntity = coordinator.CreateEntity();
        //     lineEntity.Assign<Polyline>(line);
        // }

        // {
        //     Polyline line = { {{300, 300}, {400, 350}, {450, 300}}, { {0, 1, 0, 1},  6.0f, 0.8f, 0, 0 }};
        //     Entity lineEntity = coordinator.CreateEntity();
        //     lineEntity.Assign<Polyline>(line);
        // }

        // {
        //     Polyline line = { {{50, 250}, {50, 400}}, { {0, 0, 1, 1}, 20.0f, 0.5f, 0, 0 }};
        //     Entity lineEntity = coordinator.CreateEntity();
        //     lineEntity.Assign<Polyline>(line);
        // }

        // pugi::xml_document doc;
        // doc.append_child(pugi::node_declaration);

        // auto root = doc.append_child("root");
        // coordinator.ArchiveEntity(root, farm.GetId());
        // doc.save_file("test.xml", PUGIXML_TEXT("  "));

        shared_ptr<Window> testWindow = make_shared<TestWindow>(playerData.GetComponent<ResourceStorage>(), coordinator.GetResource<std::vector<Action>>());
        renderSystem->AddWindow(testWindow);

        {
            auto createWindow = make_shared<OrbitalBodyCreationWindow>();
            renderSystem->AddWindow(createWindow);
        }

        {
            auto debugWindow = make_shared<OrbitalDebugWindow>();
            renderSystem->AddWindow(debugWindow);
        }

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

            coordinator.SetResource<SimulationTime>(SimulationTime {0.0001f, 0.0001f});

            selectionSystem->Update();
            cameraSystem->Update();
            draggingSystem->Update();
            renderSystem->Render();
            resourceSystem->Update();
            orbitSystem->Update();
            orbitPathSystem->Update();
        }
    }

    void Stop()
    {
        running = false;
    }

private:
    bool running;

    shared_ptr<RenderSystem> renderSystem;
    shared_ptr<SelectionSystem> selectionSystem;
    shared_ptr<ResourceSystem> resourceSystem;
    shared_ptr<DraggingSystem> draggingSystem;
    shared_ptr<CameraSystem> cameraSystem;
    shared_ptr<KeplerOrbitSystem> orbitSystem;
    shared_ptr<OrbitPathSystem> orbitPathSystem;

    InputManager inputManager;
};
