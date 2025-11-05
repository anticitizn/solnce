
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
        InitSSBO();
        shader.Init(shadersPath + "line_vertex.glsl", shadersPath + "line_fragment.glsl");
        shader.Activate();

        lineSegments.push_back({0, 0});
        lineSegments.push_back({100, 0});
        lineSegments.push_back({200, 0});
        lineSegments.push_back({200, 100});
    }

    void Render()
    {
        const Camera& camera = coordinator.GetResource<Camera>();

        shader.SetUniform("projection", camera.projection);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, lineSegments.size() * sizeof(glm::vec2), lineSegments.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

        shader.SetUniform("linePointsCount", (int)lineSegments.size());
        shader.SetUniform("lineWidth", 10.0f);

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, lineSegments.size() - 1);
    }

private:
    Shader shader;

    std::vector<glm::vec2> lineSegments;

    uint ssbo;
    uint maxLineSegments = 1000;

    void InitSSBO()
    {
        glGenBuffers(1, &ssbo);
    }
};
