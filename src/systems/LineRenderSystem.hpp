#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

#include <src/Shader.hpp>
#include <src/ecs/System.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Quad.hpp>

#include <external/glm/glm.hpp>
#include <external/glad/glad.h>

using namespace std;

extern Coordinator coordinator;

class LineRenderSystem : public System
{
public:
    void Init(string shadersPath)
    {
        InitSSBOs();

        shader.Init(shadersPath + "line_vertex.glsl", shadersPath + "line_fragment.glsl");
        shader.Activate();

        // sample polylines
        polylines = 
        {
            { {0, 0}, {100, 0}, {200, 0}, {200, 100}, {100, 150} },
            { {300, 300}, {400, 350}, {450, 300} },
            { {50, 250}, {50, 400} }
        };

        colors = 
        {
            {1.0f, 0.2f, 0.2f, 1.0f},  // red
            {0.2f, 1.0f, 0.2f, 1.0f},  // green
            {0.2f, 0.2f, 1.0f, 1.0f}   // blue
        };
    }

    void Render()
    {
        const Camera& camera = coordinator.GetResource<Camera>();
        shader.Activate();
        shader.SetUniform("projection", camera.projection);

        // Concatenate vertex and range buffers
        std::vector<glm::vec2> allPoints;
        std::vector<glm::uvec2> ranges;
        int totalSegments = 0;

        for (auto& line : polylines)
        {
            if (line.size() < 2) continue;

            uint startIndex = static_cast<uint>(allPoints.size());
            uint count = static_cast<uint>(line.size());

            // Append vertices
            allPoints.insert(allPoints.end(), line.begin(), line.end());

            // Store range
            ranges.push_back({ startIndex, count });
            totalSegments += static_cast<int>(count - 1);
        }

        // SSBO upload
        // Vertices
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboVertices);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     allPoints.size() * sizeof(glm::vec2),
                     allPoints.data(),
                     GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboVertices);

        // Line segment ranges
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboRanges);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     ranges.size() * sizeof(glm::uvec2),
                     ranges.data(),
                     GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboRanges);
        
        // Per-line colors
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboColors);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                    colors.size() * sizeof(glm::vec4),
                    colors.data(),
                    GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboColors);

        shader.SetUniform("lineCount", (int)ranges.size());
        shader.SetUniform("totalSegments", totalSegments);
        shader.SetUniform("lineWidth", 10.0f);
        shader.SetUniform("viewportResolution", glm::vec2(1200, 800));

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, totalSegments);
    }

private:
    Shader shader;

    std::vector<std::vector<glm::vec2>> polylines;
    std::vector<glm::vec4> colors;

    GLuint ssboVertices = 0;
    GLuint ssboRanges   = 0;
    GLuint ssboColors   = 0;

    void InitSSBOs()
    {
        glGenBuffers(1, &ssboVertices);
        glGenBuffers(1, &ssboRanges);
        glGenBuffers(1, &ssboColors);
    }
};
