
#pragma once

#include <unordered_map>

#include <src/Shader.hpp>
#include <src/WindowManager.hpp>
#include <src/ecs/System.hpp>
#include <src/components/Position.hpp>
#include <src/components/Size.hpp>
#include <src/components/Texture.hpp>

#include <external/SOIL/SOIL.h>

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
                unsigned int textureId = SOIL_load_OGL_texture(texture.filename.c_str(), 0, 0, 
                                            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
                
                filenameTextures.insert({texture.filename, textureId});
            }
        }
    }

private:
    WindowManager windowManager;
    unordered_map<string, unsigned int> filenameTextures;
    // 
};
