
#pragma once

#include <string>
#include <iostream>

#include <src/components/Pos2D.hpp>
#include <src/components/Player.hpp>
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
            auto& player = coordinator.GetComponent<Player>(entity);
            for (const auto& event : *events)
            {
                if (event.type == SDL_KEYDOWN)
                {
                    if (event.key.keysym.sym == SDLK_w)
                    {
                        pos2d.posY -= player.speed;
                    }
                    if (event.key.keysym.sym == SDLK_s)
                    {
                        pos2d.posY += player.speed;
                    }
                    if (event.key.keysym.sym == SDLK_d)
                    {
                        pos2d.posX += player.speed;
                    }
                    if (event.key.keysym.sym == SDLK_a)
                    {
                        pos2d.posX -= player.speed;
                    }
                }
            }

            GameContext::Instance().camera.position = glm::vec3(pos2d.posX, pos2d.posY, 0);
        }
    }
};


