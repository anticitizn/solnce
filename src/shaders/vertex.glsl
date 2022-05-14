#version 330 core
layout(location = 0) in vec2 quadPos;
layout(location = 1) in vec2 quadTexCoords;
layout(location = 2) in mat4 instanceMatrix;

out vec2 texCoords;

uniform mat4 cameraProjection;

void main()
{
    // camera projection is broken
    gl_Position = instanceMatrix * vec4(quadPos, 0.0f, 1.0f);
    //gl_Position = vec4(quadPos.xy, 0.0f, 1.0f);
    texCoords = quadTexCoords;
}