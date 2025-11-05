#version 430 core
in flat uint vInstanceID;
in flat uint vVertexID;
out vec4 fragColor;

void main()
{
    fragColor = vec4(float(vInstanceID % 2u),
                     float(vVertexID) / 4.0,
                     0.0,
                     1.0);
}
