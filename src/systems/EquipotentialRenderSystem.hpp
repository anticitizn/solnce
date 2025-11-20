#pragma once

#include <src/ecs/System.hpp>
#include <src/Shader.hpp>
#include <src/resources/Camera.hpp>
#include <src/components/MassiveBody.hpp>
#include <external/glm/glm.hpp>
#include <external/glad/glad.h>

extern Coordinator coordinator;

class EquipotentialRenderSystem : public System
{
public:
    void Init(const std::string& shadersPath)
    {
        shader.Init(shadersPath + "equipotential_vertex.glsl", shadersPath + "equipotential_fragment.glsl");
    }

    void Render()
    {
        const Camera& cam = coordinator.GetResource<Camera>();

        shader.Activate();

        // Camera uniforms
        shader.SetUniform("cameraPos", cam.position);
        shader.SetUniform("viewportSize", glm::vec2(cam.viewportSize.x, cam.viewportSize.y));

        // Collect body data
        std::vector<glm::vec2> positions;
        std::vector<float> masses;

        for (auto id : entities)
        {
            auto& t  = coordinator.GetComponent<Transform>(id);
            auto& mb = coordinator.GetComponent<MassiveBody>(id);
            positions.push_back(glm::vec2(t.position.x, t.position.y));
            masses.push_back(mb.mass);
        }

        int count = (int)positions.size();
        shader.SetUniform("numBodies", count);

        // Upload positions
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboBodies);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     positions.size() * sizeof(glm::vec2),
                     positions.data(),
                     GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboBodies);

        // Upload masses
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboMasses);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                     masses.size() * sizeof(float),
                     masses.data(),
                     GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboMasses);

        // Render full-screen triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

private:
    Shader shader;
    GLuint ssboBodies = 0;
    GLuint ssboMasses = 0;

public:
    EquipotentialRenderSystem()
    {
        glGenBuffers(1, &ssboBodies);
        glGenBuffers(1, &ssboMasses);
    }
};
