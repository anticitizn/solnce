
#pragma once

#include <list>

#include <SDL.h>

using namespace std;

class InputEventListener 
{
public:
    void Init(list<SDL_Event>* _events)
    {
        events = _events;
    }
    
protected:
    list<SDL_Event>* events;
};
