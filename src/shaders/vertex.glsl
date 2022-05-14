#version 330 core
layout(location = 0) in vec3 quadPos;
layout(location = 1) in vec2 quadTexCoords;
layout(location = 2) in mat4 instanceMatrix;

out vec2 texCoords;
out vec3 color;

uniform mat4 cameraProjection;

void main()
{
    texCoords = quadTexCoords;
    color = vec3(1.0f, 1.0f, 1.0f); // placeholder
    gl_Position = vec4(quadPos, 1.0f) * instanceMatrix;
}