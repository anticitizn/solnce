
#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

#include <src/Shader.hpp>
#include <src/ecs/System.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Quad.hpp>
#include <src/components/Transform.hpp>

#include <external/glm/glm.hpp>
#include <external/stb/stb_image.h>
#include <external/glad/glad.h>

using namespace std;

extern Coordinator coordinator;

class QuadRenderSystem : public System
{
public:
    void Init(string texturesPath, string shadersPath)
    {
        this->texturesPath = texturesPath;

        InitVAO();
        LoadInitialTextures();
        shader.Init(shadersPath + "quad_vertex.glsl", shadersPath + "quad_fragment.glsl");
        shader.Activate();

        shader.SetUniform("texture", 0);
    }

    Signature InitialSignature()
    {
        return coordinator.BuildSignature<Texture, Quad, Transform>();
    }

    void Render()
    {
        LoadTextures();

        const Camera& camera = coordinator.GetResource<Camera>();

        shader.Activate();
        shader.SetUniform("view", camera.view);
        shader.SetUniform("projection", camera.projection);

        unordered_map<unsigned int, vector<glm::mat4>> textureMatrixes;
        for (const auto& entity : entities)
        {
            Quad& quadComp = coordinator.GetComponent<Quad>(entity);
            Transform& tf = coordinator.GetComponent<Transform>(entity);
            Texture& textureComp = coordinator.GetComponent<Texture>(entity);

            glm::mat4 instanceMatrix = glm::mat4(1.0f);
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(tf.position.x, tf.position.y, 1.0f));
            
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(1, 1, 0));
            instanceMatrix = glm::rotate(instanceMatrix, glm::radians(tf.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
            instanceMatrix = glm::translate(instanceMatrix, glm::vec3(-1, -1, 0));

            instanceMatrix = glm::scale(instanceMatrix, glm::vec3(quadComp.sizeX / 2, quadComp.sizeY / 2, 1.0f));

            textureMatrixes[textureComp.id].push_back(instanceMatrix);
        }

        for (const auto& item : textureMatrixes)
        {
            // item.first is the texture id
            glBindTexture(GL_TEXTURE_2D, item.first);
            glBufferData(GL_ARRAY_BUFFER, textureMatrixes[item.first].size() * sizeof(glm::mat4), &textureMatrixes[item.first][0], GL_DYNAMIC_DRAW);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, textureMatrixes[item.first].size());
        }
    }

private:
    Shader shader;

    string texturesPath;

    unordered_map<string, unsigned int> filenamesTextures;

    unsigned int quadVBO;
    unsigned int instanceVBO;
    unsigned int VAO;

    int maxQuads = 100;

    const float quadVertices[24] = 
    {
        // positions,  texture coordinates
        -1.0f, -1.0f,  0.0f,  0.0f, // lower left
        -1.0f,  1.0f,  0.0f,  1.0f, // upper left
         1.0f,  1.0f,  1.0f,  1.0f, // upper right
        
        -1.0f, -1.0f,  0.0f,  0.0f, // lower left
         1.0f, -1.0f,  1.0f,  0.0f, // lower right
         1.0f,  1.0f,  1.0f,  1.0f  // upper right
    };

    void InitVAO()
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Shared quad vertex data
        glGenBuffers(1, &quadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STREAM_DRAW);

        // Quad position vertices
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        
        // Quad texture coordinates
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glVertexAttribDivisor(0, 0);
        glVertexAttribDivisor(1, 0);

        // Per-instance data
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        // Initialize with empty buffer; sizeof(Quad) should be the same as sizeof(float) * 9
        glBufferData(GL_ARRAY_BUFFER, maxQuads * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);

        // Instance scale/transform/rotation matrix - divided into 4*vec4
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        /* 
        // Temporarily disabled
        // Color vertices
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(4 * sizeof(glm::vec4)));
        */

        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        //glVertexAttribDivisor(7, 1);
        
    }

    void LoadInitialTextures()
    {
        // Texture 1 serves as missing texture image
        LoadTexture("missing-texture.png");
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
        
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load((texturesPath + filename).c_str(), &imgWidth, &imgHeight, &nrChannels, 0);

        if (data)
        {
            // Only generate a texture if the file actually loads
            glGenTextures(1, &texture);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            // Set some texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            uint32_t colorspace = GL_RGBA;
            if (nrChannels == 3)
            {
                colorspace = GL_RGB;
            }

            glTexImage2D(GL_TEXTURE_2D, 0, colorspace, imgWidth, imgHeight, 0, colorspace, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            // With texture = 1 the system won't try to load the texture any more
            cout << "WARNING: Failed to load texture | " << filename  << " | at " << texturesPath + filename << endl;
            return 1;
        }

        stbi_image_free(data);

        filenamesTextures.insert({filename, texture});
        cout << "Texture: " << texture << endl;
        return texture;
    }
};
