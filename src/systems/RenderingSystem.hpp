
#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

#include <src/Shader.hpp>
#include <src/WindowManager.hpp>
#include <src/ecs/System.hpp>
#include <src/components/Position.hpp>
#include <src/components/Size.hpp>
#include <src/components/Texture.hpp>

#include <external/stb/stb_image.h>
#include <external/glad/glad.h>

using namespace std;

extern Coordinator coordinator;

class RenderingSystem : public System
{
public:
    void Init(string TexturesPath)
    {
        texturesPath = TexturesPath;
        windowManager.Init("SOLNCE", false);
        InitOpenGL();
        LoadInitialTextures();
        Render();
    }

    void Render()
    {
        LoadTextures();

    }

private:
    string texturesPath;
    WindowManager windowManager;
    unordered_map<string, unsigned int> filenamesTextures;

    void InitOpenGL()
    {
        glViewport(0, 0, windowManager.GetContextWidth(), windowManager.GetContextHeight());
    }

    void LoadInitialTextures()
    {
        // textures 0 and 1 serve as missing texture images
        LoadTexture("missing-texture");
        LoadTexture("missing-texture");
    }

    void LoadTextures()
    {
        for (const auto& entity : entities)
        {
            auto& textureComponent = coordinator.GetComponent<Texture>(entity);
            if (textureComponent.textureId == 0)
            {
                unsigned int texture = LoadTexture(textureComponent.filename);
                textureComponent.textureId = texture;
            }
        }
    }

    unsigned int LoadTexture(string filename)
    {
        if (!(filenamesTextures.find(filename) == filenamesTextures.end()))
        {
            return filenamesTextures[filename];
        }
        unsigned int texture;

        // set some texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // load the image file
        int imgWidth, imgHeight, nrChannels;
        
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load((texturesPath + filename + ".jpg").c_str(), &imgWidth, &imgHeight, &nrChannels, 0);
    
        if (data)
        {
            // only generate a texture if the file actually loaded
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            // with texture = 1 the system won't try to load the texture anymore
            cout << "ERROR: Failed to load texture " << filename << endl;
            return 1;
        }

        stbi_image_free(data);

        filenamesTextures.insert({filename, texture});

        return texture;
    }
};
