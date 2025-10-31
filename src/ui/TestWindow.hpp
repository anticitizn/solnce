
#pragma once

#include <src/components/ResourceStorage.hpp>
#include <src/ui/Window.hpp>

using namespace std;

class TestWindow : public Window
{
public:
    TestWindow(ResourceStorage& resourceStorage, std::vector<Action>& actions) : resourceStorage(resourceStorage), actions(actions) {};
    virtual void Draw()
    {
        for (auto action : actions)
        {
            if (action.type == CursorMotion)
            {
                mousePos = action.position;
            }
        }
        if (display)
        {
            ImGui::Begin("TestWindow");
            ImGui::Text("This is a test window");
            ImGui::SliderFloat("Ore", &resourceStorage.ore, 0.0f, 1000.0f);
            ImGui::SliderFloat("Food", &resourceStorage.food, 0.0f, 1000.0f);
            ImGui::SliderFloat("Alloys", &resourceStorage.alloys, 0.0f, 1000.0f);
            ImGui::Text("X: %d", (int)mousePos.x);
            ImGui::Text("Y: %d", (int)mousePos.y);
            ImGui::End();
        }
    }

private:
    ResourceStorage& resourceStorage;
    std::vector<Action>& actions;
    glm::vec2 mousePos;
};
