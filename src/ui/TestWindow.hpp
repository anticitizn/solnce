
#pragma once

#include <src/components/ResourceStorage.hpp>
#include <src/ui/Window.hpp>

using namespace std;

class TestWindow : public Window
{
public:
    TestWindow(ResourceStorage& resourceStorage, MouseState& mouseState) : resourceStorage(resourceStorage), mouseState(mouseState) {};
    virtual void Draw()
    {
        if (display)
        {
            ImGui::Begin("TestWindow");
            ImGui::Text("This is a test window");
            ImGui::SliderFloat("Ore", &resourceStorage.ore, 0.0f, 1000.0f);
            ImGui::SliderFloat("Food", &resourceStorage.food, 0.0f, 1000.0f);
            ImGui::SliderFloat("Alloys", &resourceStorage.alloys, 0.0f, 1000.0f);
            ImGui::Text("X: %d", mouseState.x);
            ImGui::Text("X: %d", mouseState.y);
            ImGui::End();
        }
    }

private:
    ResourceStorage& resourceStorage;
    MouseState& mouseState;
};
