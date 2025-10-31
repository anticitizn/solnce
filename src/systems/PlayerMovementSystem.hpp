
#pragma once

#include <string>
#include <iostream>
#include <unordered_set>

#include <src/components/Pos2D.hpp>
#include <src/components/Player.hpp>
#include <src/ecs/System.hpp>
#include <src/context/GameContext.hpp>

#include <external/glm/glm.hpp>
#include <SDL.h>

using namespace std;

extern Coordinator coordinator;

class PlayerMovementSystem : public System
{
public:
    void Update()
    {
        for (const auto& entity : entities)
        {
            auto& pos2d = coordinator.GetComponent<Pos2D>(entity);
            auto& player = coordinator.GetComponent<Player>(entity);

            // Reset velocity every frame
            player.velocity = {0, 0};

            std::vector<Action> actions = coordinator.GetResource<std::vector<Action>>();
            for (const auto& action : actions)
            {
                if (action.type == MoveCamera && action.phase != Stopped)
                {
                    player.velocity.x += action.delta.x * player.speed;
                    player.velocity.y += action.delta.y * player.speed;
                }
            }

            pos2d.posX += player.velocity.x;
            pos2d.posY += player.velocity.y;

            gameContext.camera.position = glm::vec3(pos2d.posX, pos2d.posY, 0);
        }
    }
};


