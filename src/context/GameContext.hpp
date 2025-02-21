
#pragma once

#include <src/context/Camera.hpp>

// This class saves some data that is supposed to be a singleton
// For example camera data, since there is only 1 camera in the game
class GameContext {
public:
    static GameContext& Instance() 
    {
        static GameContext instance;
        return instance;
    }

    Camera camera;

    // Prevent copying
    GameContext(const GameContext&) = delete;
    void operator=(const GameContext&) = delete;

private:
    GameContext() {}
};
