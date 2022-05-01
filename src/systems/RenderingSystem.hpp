
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
        for (unsigned int i = 0; i < 6; i++)
        {
            shaderManager.SetUniform(("quadVertices[" + std::to_string(i) + "]"), 
                                      quadVertices[i*4], quadVertices[i*4 +1], quadVertices[i*4 + 2], quadVertices[i*4 + 3]);
        }  
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

            float vals[10] = {quadComp.posX, quadComp.posY, quadComp.posZ, quadComp.sizeX, quadComp.sizeY, quadComp.r, quadComp.g, quadComp.b, quadComp.rot};
            for (int i = 0; i < 10; i++) 
            {
                texturesVectors[textureComp.id].push_back(vals[i]);
            }
        }

        for (const auto& item : texturesVectors)
        {
            // item.first is the texture id
            glBindTexture(GL_TEXTURE_2D, item.first);
            glBufferData(GL_ARRAY_BUFFER, texturesVectors[item.first].size() * sizeof(float), &texturesVectors[item.first][0], GL_DYNAMIC_DRAW);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 1, texturesVectors[item.first].size() / 9); // divide by 9 because each quad has 9 float values
        }
    }

private:
    string texturesPath;
    string shadersPath;
    WindowManager windowManager;
    ShaderManager shaderManager;
    unordered_map<string, unsigned int> filenamesTextures;
    unsigned int VBO;
    unsigned int VAO;
    float quadVertices[32] = {
        // positions,  texture coordinates
        -1.0f, -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,  1.0f,
        
        -1.0f, -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,  1.0f
    };

    void InitOpenGL()
    {
        glViewport(0, 0, windowManager.GetContextWidth(), windowManager.GetContextHeight());

        // set some texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        InitVAO();
    }

    void InitVAO()
    {
        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // position
        glEnableVertexAttribArray(0);
        glVertexAttribDivisor(0, 1);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(3 * sizeof(float))); // size
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(5 * sizeof(float))); // color
        glEnableVertexAttribArray(2);
        glVertexAttribDivisor(2, 1);

        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(8 * sizeof(float))); // rotation
        glEnableVertexAttribArray(3);
        glVertexAttribDivisor(3, 1);
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
            glActiveTexture(GL_TEXTURE0);
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
