
#pragma once

#include <external/glm/glm.hpp>

struct Camera
{
    // Camera center (m)
    glm::dvec2 position;

    double metersPerPixel = 1e9;
    double minMetersPerPixel = 1.0;
    double maxMetersPerPixel = 1.0e12;

    double moveSpeed = 1.0;
    double zoomSpeed = 1.1;

    glm::ivec2 viewportSize;

    glm::dmat4 view = glm::mat4(1.0f);
    glm::dmat4 projection = glm::mat4(1.0f);
    glm::dmat4 inverseProjection = glm::mat4(1.0f);
};
