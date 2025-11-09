#version 430 core

out vec4 vColor;

uniform mat4  projection;
uniform int   lineCount;        // number of polylines
uniform int   totalSegments;    // number of all segments across all lines

// Concatenated all points for all polylines, 
layout(std430, binding = 0) buffer LineVertices 
{
    vec2 vertex[];
};

// For each polyline: (startIndex into vertex[], vertexCount for that polyline)
layout(std430, binding = 1) buffer LineRanges 
{
    uvec2 lineRanges[]; // .x = start, .y = count
};

struct LineAttribute 
{
    vec4 color;
    float width;
    float fade;
    float pad1;   // padding to keep 16-byte alignment (std430 packing rules)
    float pad2;
};

layout(std430, binding = 2) buffer LineAttributes
{
    LineAttribute attrib[];
};



vec2 perp(vec2 v) 
{ 
    return vec2(-v.y, v.x); 
}

vec2 safeNormalize(vec2 v) 
{
    float len = length(v);
    if (len > 0.0)
    {
        return v / len;
    }
    else
    {
        vec2(0.0);
    }
}

// Returns (lineID, localSegIndex) for a global segment index
// performs a linear scan
uvec2 findLineAndLocalSeg(uint segIndex) 
{
    for (uint i = 0u; i < uint(lineCount); ++i) 
    {
        uvec2 r = lineRanges[i];
        uint segs = (r.y > 0u) ? (r.y - 1u) : 0u;

        if (segIndex < segs) 
        {
            return uvec2(i, segIndex);
        }

        segIndex -= segs;
    }

    // Fallback to the last line, which shouldn't happen if totalSegments is set correctly
    return uvec2(uint(max(0, lineCount - 1)), 0u);
}

// Compute a left-side miter offset at a joint from dA->dB, constrained by nRef
vec2 miterOffset(vec2 dA, vec2 dB, vec2 nRef, float halfWidth) 
{
    // angle bisector tangent
    vec2 t = safeNormalize(dA + dB);             

    // 180 deg / degenerate line -> bevel
    if (t == vec2(0.0)) 
    {
        return nRef * halfWidth;
    } 

    // miter direction is perpendicular to bisector
    vec2 mDir = perp(t);                 
    float denom = dot(mDir, nRef);

    // avoid blowing up
    if (abs(denom) < 1e-5) 
    {
        return nRef * halfWidth; 
    }

    vec2 offset = mDir * (halfWidth / denom);

    // Fall back to a bevel in the case of extremely long miters that can result from sharp angles between line segments
    const float MITER_LIMIT = 4.0;
    if (length(offset) > MITER_LIMIT * halfWidth) 
    {
        return nRef * halfWidth;
    }
    return offset;
}

void main() 
{
    uint gSeg = gl_InstanceID; // global segment index across all lines
    uint vid  = gl_VertexID;   // 0..3 in the strip

    // Map to a specific polyline and local segment
    uvec2 lineLocal = findLineAndLocalSeg(gSeg);
    uint lineID     = lineLocal.x;
    uint localSeg   = lineLocal.y;

    vColor  = attrib[lineID].color;
    //vFade  = attrib[lineID].fade;

    float lineWidth = attrib[lineID].width;
    float halfW = 0.5 * lineWidth;

    uvec2 range = lineRanges[lineID];
    uint base   = range.x; // first vertex index of this line
    uint vCount = range.y; // number of vertices in this line
    uint lastLocalSeg = (vCount >= 1u) ? (vCount - 2u) : 0u;

    // Endpoints for this segment
    vec2 P0 = vertex[base + localSeg];
    vec2 P1 = vertex[base + localSeg + 1u];

    // Directions: prev (into P0), curr (P0->P1), next (out of P1)
    vec2 dCurr = safeNormalize(P1 - P0);

    vec2 dPrev;
    if (localSeg == 0u) 
    {
        dPrev = dCurr; // start cap
    } 
    else 
    {
        vec2 Pm1 = vertex[base + localSeg - 1u];
        dPrev = safeNormalize(P0 - Pm1);
    }

    vec2 dNext;
    if (localSeg == lastLocalSeg) 
    {
        dNext = dCurr; // end cap
    } else 
    {
        vec2 P2 = vertex[base + localSeg + 2u];
        dNext = safeNormalize(P2 - P1);
    }

    // Left normals (reference)
    vec2 nPrev = perp(dPrev);
    vec2 nNext = perp(dNext);

    // Start/end offsets (left/right)
    vec2 startLeft  = miterOffset(dPrev, dCurr, nPrev, halfW);
    vec2 startRight = -startLeft;

    vec2 endLeft  = miterOffset(dCurr, dNext, nNext, halfW);
    vec2 endRight = -endLeft;

    // Triangle strip:
    // 0: P0 + left, 1: P0 + right, 2: P1 + left, 3: P1 + right
    vec2 pos;
    if      (vid == 0u) pos = P0 + startLeft;
    else if (vid == 1u) pos = P0 + startRight;
    else if (vid == 2u) pos = P1 + endLeft;
    else                pos = P1 + endRight;

    gl_Position = projection * vec4(pos, 0.0, 1.0);
}
