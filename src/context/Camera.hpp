
#pragma once

#include <external/glm/glm.hpp>

struct Camera
{
    glm::vec3 position;

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 viewProjection = glm::mat4(1.0f);
    glm::mat4 inverseViewProjection = glm::mat4(1.0f);
};
