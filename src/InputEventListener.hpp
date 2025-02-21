
#pragma once

#include <list>

#include <SDL.h>
#include <src/InputManager.hpp>

using namespace std;

extern InputManager inputManager;

class InputEventListener 
{
public:
    InputEventListener()
    {
        events = inputManager.GetEvents();
    }
    
protected:
    list<SDL_Event>* events;
};
