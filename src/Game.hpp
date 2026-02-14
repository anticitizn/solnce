
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
#include <src/systems/SOISwitchingSystem.hpp>
#include <src/systems/SOICalculationSystem.hpp>
#include <src/resources/SimulationTime.hpp>
#include <src/io/InputManager.hpp>

#include <src/ui/TestWindow.hpp>
#include <src/ui/TimeWindow.hpp>
#include <src/ui/OrbitalBodyCreationWindow.hpp>
#include <src/ui/OrbitalInspectorWindow.hpp>

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
            Signature signature = coordinator.BuildSignature<ResourceGenerator>();
            coordinator.SetSystemSignature<ResourceSystem>(signature);
        }

        draggingSystem = coordinator.RegisterSystem<DraggingSystem>();
        selectionSystem = coordinator.RegisterSystem<SelectionSystem>();
        orbitPathSystem = coordinator.RegisterSystem<OrbitPathSystem>();
        cameraSystem = coordinator.RegisterSystem<CameraSystem>();
        orbitSystem = coordinator.RegisterSystem<KeplerOrbitSystem>();
        soiCalculationSystem = coordinator.RegisterSystem<SOICalculationSystem>();
        soiSwitchingSystem = coordinator.RegisterSystem<SOISwitchingSystem>();

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

        // pugi::xml_document doc;
        // doc.append_child(pugi::node_declaration);

        // auto root = doc.append_child("root");
        // coordinator.ArchiveEntity(root, farm.GetId());
        // doc.save_file("test.xml", PUGIXML_TEXT("  "));

        // shared_ptr<Window> testWindow = make_shared<TestWindow>(playerData.GetComponent<ResourceStorage>(), coordinator.GetResource<std::vector<Action>>());
        // renderSystem->AddWindow(testWindow);

        auto orbitalInspector = coordinator.RegisterSystem<OrbitalInspectorWindow>();
        renderSystem->AddWindow(orbitalInspector);

        auto timeWindow = make_shared<TimeWindow>();
        renderSystem->AddWindow(timeWindow);

        // auto debugWindow = coordinator.RegisterSystem<OrbitalDebugWindow>();
        // renderSystem->AddWindow(debugWindow);

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

            uint64_t msSinceInit = SDL_GetTicks();
            SimulationTime simTime = coordinator.GetResource<SimulationTime>();

            simTime.real_dt = msSinceInit - simTime.real_time;
            simTime.real_time = msSinceInit;

            simTime.sim_dt = simTime.real_dt * simTime.sim_time_factor;
            simTime.sim_time += simTime.sim_dt;

            coordinator.SetResource<SimulationTime>(simTime);

            resourceSystem->Update();
            orbitSystem->Update();
            soiCalculationSystem->Update();
            soiSwitchingSystem->Update();
            orbitPathSystem->Update();

            selectionSystem->Update();
            cameraSystem->Update();
            draggingSystem->Update();

            renderSystem->Render();
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
    shared_ptr<SOICalculationSystem> soiCalculationSystem;
    shared_ptr<SOISwitchingSystem> soiSwitchingSystem;

    InputManager inputManager;
};
