#version 330 core

out vec4 fragColor;

in vec2 texCoords;
in vec3 color;

uniform sampler2D texture;

void main()
{
    fragColor = texture(texture, texCoords) * vec4(color / 255.0f, 1.0f);
}