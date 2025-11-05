#version 430 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D quadTex;

void main()
{
    //fragColor = vec4(1.0f);
    fragColor = texture(quadTex, texCoords);
}