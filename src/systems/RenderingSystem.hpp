
#pragma once

#include <unordered_map>

#include <src/Shader.hpp>
#include <src/WindowManager.hpp>
#include <src/ecs/System.hpp>
#include <src/components/Position.hpp>
#include <src/components/Size.hpp>
#include <src/components/Texture.hpp>

#include <external/stb/stb_image.h>

using namespace std;

extern Coordinator coordinator;

class RenderingSystem : public System
{
public:
    void Init()
    {
        windowManager.Init("SOLNCE", false);
        Render();
    }

    void Render()
    {
        LoadTextures();

    }

    void LoadTextures()
    {
        for (const auto& entity : entities)
        {
            auto const& texture = coordinator.GetComponent<Texture>(entity);
            if (filenameTextures.find(texture.filename) == filenameTextures.end())
            {
                // load image with stb and generate texture
                //filenameTextures.insert({texture.filename, textureId});
            }
        }
    }

private:
    WindowManager windowManager;
    unordered_map<string, unsigned int> filenameTextures;
};
