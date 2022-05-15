#version 330 core
layout(location = 0) in vec2 quadPos;
layout(location = 1) in vec2 quadTexCoords;
layout(location = 2) in mat4 instanceMatrix;

out vec2 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * instanceMatrix * vec4(quadPos, 0.0f, 1.0f);
    texCoords = quadTexCoords;
}