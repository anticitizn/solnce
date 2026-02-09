
#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

#include <src/WindowManager.hpp>
#include <src/ecs/System.hpp>
#include <src/ui/Window.hpp>
#include <src/resources/CursorPos.hpp>
#include <src/systems/QuadRenderSystem.hpp>
#include <src/systems/LineRenderSystem.hpp>
#include <src/systems/EquipotentialRenderSystem.hpp>

#include <external/glm/glm.hpp>
#include <external/glad/glad.h>

#include <external/imgui/imgui.h>
#include <external/imgui/imgui_impl_sdl3.h>
#include <external/imgui/imgui_impl_opengl3.h>

using namespace std;

extern Coordinator coordinator;

class RenderSystem : public System
{
public:
    Signature InitialSignature()
    {
        return coordinator.BuildSignature<>();
    }

    void Init()
    {
        windowManager.Init("SOLNCE", true, 1920, 1080, 0, 0);

        Camera& camera = coordinator.GetResource<Camera>();
        windowManager.GetContextSize(&camera.viewportSize.x, &camera.viewportSize.y);

        InitOpenGL(camera.viewportSize.x, camera.viewportSize.y);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        quadSystem = coordinator.RegisterSystem<QuadRenderSystem>();
        quadSystem->Init("assets/", "src/shaders/");

        lineSystem = coordinator.RegisterSystem<LineRenderSystem>();
        lineSystem->Init("src/shaders/");
    }

    void Render()
    {
        glDisable(GL_CULL_FACE);
        BeginFrame();
        
        // Call all rendering subsystems
        lineSystem->Render();
        quadSystem->Render();
        
        RenderUI();

        EndFrame();
    }

    void AddWindow(shared_ptr<Window> window)
    {
        windows.push_back(window);
    }

private:    
    WindowManager windowManager;

    glm::mat4 viewMatrix;
    glm::mat4 cameraProjection;

    vector<shared_ptr<Window>> windows;
    shared_ptr<QuadRenderSystem> quadSystem;
    shared_ptr<LineRenderSystem> lineSystem;

    void InitOpenGL(int width, int height)
    {
        // Enable antialiasing
        glEnable(GL_MULTISAMPLE);
        glViewport(0, 0, width, height);
    }

    void RenderUI()
    {
        windowManager.Refresh();

        // Render UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        for (long unsigned int i = 0; i < windows.size(); i++)
        {
            windows[i]->Draw();
        }
        
        // ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void BeginFrame()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void EndFrame()
    {
        windowManager.SwapBuffers();
    }
};
