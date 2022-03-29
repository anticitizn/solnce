#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aColor;

out vec2 texCoords;
out vec3 color;

uniform mat4 cameraProjection;

void main()
{
    texCoords = aTexCoords;
    gl_Position = cameraProjection * vec4(aPos.xyz, 1.0f);
}