
#pragma once

#include <external/glm/glm.hpp>

struct Camera
{
    // Camera center (m)
    glm::dvec2 position;

    double metersPerPixel = 1.0e6;
    double minMetersPerPixel = 1.0;
    double maxMetersPerPixel = 1.0e12;

    double moveSpeed = 1.0;
    double zoomSpeed = 1.1;

    glm::vec2 viewportSize;

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 inverseProjection = glm::mat4(1.0f);
};
