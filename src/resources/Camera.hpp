
#pragma once

#include <external/glm/glm.hpp>

struct Camera
{
    glm::vec3 position;

    float moveSpeed;
    float zoomSpeed;

    glm::vec2 viewportSize;

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 inverseProjection = glm::mat4(1.0f);
};
