
#pragma once

#include <external/imgui/imgui.h>

using namespace std;

class Window 
{
public:
    bool display = true;
    virtual void Draw() = 0;

private:
    
};
