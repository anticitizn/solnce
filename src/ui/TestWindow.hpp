
#pragma once

#include <src/components/Quad.hpp>
#include <src/ui/Window.hpp>

using namespace std;

class TestWindow : public Window
{
public:
    TestWindow(Quad& sourceQuad) : quad(sourceQuad) {};
    virtual void Draw()
    {
        if (display)
        {
            ImGui::Begin("TestWindow");
            ImGui::Text("This is a test window");
            ImGui::SliderFloat("Pos x", &quad.posX, 0.0f, 600.0f);
            ImGui::SliderFloat("Pos y", &quad.posY, 0.0f, 600.0f);
            ImGui::SliderFloat("Size x", &quad.sizeX, 0.0f, 600.0f);
            ImGui::SliderFloat("Size y", &quad.sizeY, 0.0f, 600.0f);
            ImGui::SliderFloat("Rotation", &quad.rot, 0.0f, 360.0f);
            ImGui::End();
        }
    }

private:
    Quad& quad;
};
