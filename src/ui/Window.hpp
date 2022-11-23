
#pragma once

#include <external/imgui/imgui.h>

using namespace std;

class Window 
{
public:
    bool display = true;
    virtual void Draw()
    {
        if (display)
        {
            ImGui::Begin("Window");
            ImGui::Text("This is an empty window");
            ImGui::End();
        }
    }

private:
    
};
