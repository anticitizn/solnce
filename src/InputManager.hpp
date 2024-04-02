
#pragma once

#include <list>

#include <SDL.h>

using namespace std;

class InputManager
{
public:
    InputManager()
    {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    void Update()
    {
        // Prevent the list of pending events from getting filled with
        // events that no one is interested in
        // Events should usually be handled within a single frame so this should be OK
        if (events.size() > 0)
        {
            events.clear();
        }

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            events.push_back(event);
        }
    }

    list<SDL_Event>* GetEvents()
    {
        return &events;
    }

private:
    list<SDL_Event> events;
};
