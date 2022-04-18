
#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

#include <src/ShaderManager.hpp>
#include <src/WindowManager.hpp>
#include <src/ecs/System.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Quad.hpp>

#include <external/stb/stb_image.h>
#include <external/glad/glad.h>

using namespace std;

extern Coordinator coordinator;

class RenderingSystem : public System
{
public:
    void Init(string TexturesPath, string ShadersPath)
    {
        shadersPath = ShadersPath;
        texturesPath = TexturesPath;

        windowManager.Init("SOLNCE", false);
        InitOpenGL();
        LoadInitialTextures();
        shaderManager.Init(shadersPath);
        Render();
    }

    void Render()
    {
        LoadTextures();

        unordered_map<unsigned int, vector<float>> texturesVectors;
        for (const auto& entity : entities)
        {
            Quad& quadComp = coordinator.GetComponent<Quad>(entity);
            Texture& textureComp = coordinator.GetComponent<Texture>(entity);

            // fuck my life, there has to be a better way to do this
            float vals[10] = {quadComp.posX, quadComp.posY, quadComp.posZ, quadComp.sizeX, quadComp.sizeY, quadComp.r, quadComp.g, quadComp.b, quadComp.rot};
            for (int i = 0; i < 10; i++) 
            {
                texturesVectors[textureComp.id].push_back(vals[i]);
            }
            
            
        }
    }

private:
    string texturesPath;
    string shadersPath;
    WindowManager windowManager;
    ShaderManager shaderManager;
    unordered_map<string, unsigned int> filenamesTextures;

    void InitOpenGL()
    {
        glViewport(0, 0, windowManager.GetContextWidth(), windowManager.GetContextHeight());

        // set some texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
            if (textureComponent.id == 0)
            {
                unsigned int texture = LoadTexture(textureComponent.filename);
                textureComponent.id = texture;
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

        // load the image file
        int imgWidth, imgHeight, nrChannels;
        
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load((texturesPath + filename + ".png").c_str(), &imgWidth, &imgHeight, &nrChannels, 0);
    
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
            cout << "ERROR: Failed to load texture | " << filename  << " | at " << texturesPath + filename + ".png" << endl;
            return 1;
        }

        stbi_image_free(data);

        filenamesTextures.insert({filename, texture});

        return texture;
    }
};
