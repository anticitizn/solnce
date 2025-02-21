
#pragma once

#include <string>
#include <iostream>

#include <src/components/Pos2D.hpp>
#include <src/ecs/System.hpp>
#include <external/glm/glm.hpp>

#include <SDL.h>

using namespace std;

extern Coordinator coordinator;

class PlayerMovementSystem : public System, public InputEventListener
{
public:
    void Update()
    {
        for (const auto& entity : entities)
        {
            auto& pos2d = coordinator.GetComponent<Pos2D>(entity);
            for (const auto& event : *events)
            {
                if (event.type == SDL_KEYDOWN)
                {
                    if (event.key.keysym.sym == SDLK_UP)
                    {
                        pos2d.posY -= 1;
                    }
                    if (event.key.keysym.sym == SDLK_DOWN)
                    {
                        pos2d.posY += 1;
                    }
                    if (event.key.keysym.sym == SDLK_RIGHT)
                    {
                        pos2d.posX += 1;
                    }
                    if (event.key.keysym.sym == SDLK_LEFT)
                    {
                        pos2d.posX -= 1;
                    }
                }
            }

            GameContext::Instance().camera.position = glm::vec3(pos2d.posX, pos2d.posY, 0);
        }
    }
};


