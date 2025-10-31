
#pragma once

#include <external/glm/glm.hpp>

enum ActionType
{
    Select, 
    DragCamera,
    MoveCamera,
    CursorMotion,
    Zoom
};

enum ActionPhase
{
    Started, 
    Held, 
    Stopped
};

// Position, delta and value can be optionally populated depending on the action type
struct Action
{
    ActionType type;
    ActionPhase phase;
    glm::vec2 position;
    glm::vec2 delta;
    float value;
};
