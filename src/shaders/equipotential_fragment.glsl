#version 430 core

layout(std430, binding = 0) buffer BodyPositions {
    vec2 bodyPos[];
};

layout(std430, binding = 1) buffer BodyMasses {
    float bodyMass[];
};

uniform int   numBodies;
uniform vec3  cameraPos;
uniform vec2  viewportSize;

out vec4 fragColor;

vec2 getWorldPos()
{
    vec2 uv = vec2(gl_FragCoord.x / viewportSize.x,
                   1.0 - gl_FragCoord.y / viewportSize.y);

    vec2 screen = (uv - 0.5) * viewportSize;
    screen /= cameraPos.z;
    return screen + cameraPos.xy;
}

mat2 rot(float a)
{
    float c = cos(a), s = sin(a);
    return mat2(c, -s, s, c);
}

float isolines(float v, float spacing)
{
    float x = fract(v / spacing);
    float d = min(x, 1.0 - x);
    return 1.0 - smoothstep(0.0, 0.1, d);
}

void main()
{
    vec2 p = getWorldPos();

    // Current inertial positions of the bodies
    vec2 pos0_inertial = bodyPos[0];
    vec2 pos1_inertial = bodyPos[1];

    // Compute the barycenter
    float M0 = bodyMass[0];
    float M1 = 1e19;
    float Minv = 1.0 / (M0 + M1);
    vec2 center_inertial = (M0 * pos0_inertial + M1 * pos1_inertial) * Minv;

    // Vector from star->planet in inertial frame
    vec2 axis = pos1_inertial - pos0_inertial;

    // Angle needed to align star->planet with +X axis
    float angle = -atan(axis.y, axis.x);

    // Rotation matrix: inertial -> rotating frame
    mat2 R = rot(angle);

    // Rotate sample point and body positions
    vec2 p_rot   = R * (p - center_inertial);
    vec2 pos0    = R * (pos0_inertial - center_inertial);
    vec2 pos1    = R * (pos1_inertial - center_inertial);
    vec2 center  = vec2(0.0); // barycenter is now origin

    // Effective potential
    float potential = 0.0;

    // gravitational potentials
    {
        vec2 d0 = p_rot - pos0;
        float r0 = sqrt(dot(d0, d0) + 1.0);
        potential += -M0 / r0;

        vec2 d1 = p_rot - pos1;
        float r1 = sqrt(dot(d1, d1) + 1.0);
        potential += -M1 / r1;
    }

    float omg = 6e6;

    // centrifugal potential in rotating frame
    potential += -0.5 * omg * omg * dot(p_rot, p_rot);

    float potentialScale = 1e-15;

    // scale + log compress
    float v = log((-potential) * potentialScale + 1.0);

    // isolines
    float iso = isolines(v, 0.05);

    fragColor = vec4(vec3(iso/3), 1.0);
}
