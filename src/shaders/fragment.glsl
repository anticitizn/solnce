#version 330 core

out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D texture;

void main()
{
    fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}