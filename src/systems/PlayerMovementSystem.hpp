
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

class PlayerMovementSystem : public System, public InputEventListener
{
public:
    void Update()
    {
        for (const auto& event : *events)
        {
            if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
            {
                heldKeys.insert(event.key.keysym.sym);
            }
            else if (event.type == SDL_KEYUP)
            {
                heldKeys.erase(event.key.keysym.sym);
            }
        }

        for (const auto& entity : entities)
        {
            auto& pos2d = coordinator.GetComponent<Pos2D>(entity);
            auto& player = coordinator.GetComponent<Player>(entity);

            // Reset velocity every frame
            player.velocity = {0, 0};

            if (heldKeys.count(SDLK_w)) player.velocity.y -= player.speed;
            if (heldKeys.count(SDLK_s)) player.velocity.y += player.speed;
            if (heldKeys.count(SDLK_d)) player.velocity.x += player.speed;
            if (heldKeys.count(SDLK_a)) player.velocity.x -= player.speed;

            pos2d.posX += player.velocity.x;
            pos2d.posY += player.velocity.y;

            gameContext.camera.position = glm::vec3(pos2d.posX, pos2d.posY, 0);
        }
    }

private:
    std::unordered_set<SDL_Keycode> heldKeys;
};


