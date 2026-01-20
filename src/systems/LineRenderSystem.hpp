#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

#include <src/Shader.hpp>
#include <src/ecs/System.hpp>
#include <src/components/Polyline.hpp>

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
    }

    Signature InitialSignature()
    {
        return coordinator.BuildSignature<Polyline>();
    }

    void Render()
    {
        const Camera& camera = coordinator.GetResource<Camera>();
        shader.Activate();
        shader.SetUniform("projection", camera.projection);

        // Concatenate vertex and range buffers
        std::vector<glm::vec2> allPoints;
        std::vector<glm::uvec2> ranges;
        std::vector<PolylineAttribute> attribs;
        int totalSegments = 0;

        for (const auto& entity : entities)
        {
            Polyline& quadComp = coordinator.GetComponent<Polyline>(entity);

            if (quadComp.segments.size() < 2) continue;

            uint startIndex = static_cast<uint>(allPoints.size());
            uint count = static_cast<uint>(quadComp.segments.size());

            // Append vertices
            allPoints.insert(allPoints.end(), quadComp.segments.begin(), quadComp.segments.end());

            // Store the range
            ranges.push_back({ startIndex, count });
            totalSegments += static_cast<int>(count - 1);
            
            // Store the per-line attributes
            attribs.push_back(quadComp.attribute);
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
        
        // Per-line attributes
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboAttribs);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                    attribs.size() * sizeof(PolylineAttribute),
                    attribs.data(),
                    GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboAttribs);


        shader.SetUniform("lineCount", (int)ranges.size());
        shader.SetUniform("totalSegments", totalSegments);
        shader.SetUniform("lineWidth", 10.0f);
        shader.SetUniform("viewportResolution", glm::vec2(1200, 800));

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, totalSegments);
    }

private:
    Shader shader;

    GLuint ssboVertices = 0;
    GLuint ssboRanges   = 0;
    GLuint ssboAttribs   = 0;

    void InitSSBOs()
    {
        glGenBuffers(1, &ssboVertices);
        glGenBuffers(1, &ssboRanges);
        glGenBuffers(1, &ssboAttribs);
    }
};
