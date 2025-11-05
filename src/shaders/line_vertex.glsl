#version 430 core

uniform mat4 projection;
uniform uint linePointsCount;
uniform float lineWidth;

layout(std430, binding = 0) buffer TVertex
{
   vec2 vertex[]; 
};

out flat uint vInstanceID;
out flat uint vVertexID;

void main()
{
   uint instanceIdx = gl_InstanceID;
   uint vertexIdx = gl_VertexID;

   vInstanceID = gl_InstanceID;
   vVertexID = gl_VertexID;
   
   vec2 dir = normalize(vertex[instanceIdx] - vertex[instanceIdx + 1]);
   vec2 perp = vec2(-dir.y, dir.x) * (lineWidth * 0.5);

   // We use this to check if we have reached the last or second last item of the vector (so there is no vertex[instanceIdx + 2] at all)
   // If this is the case, the joint/miter will have the same angle as the first line segment point, i.e. it will just be rectangular
   uint nextInstance = min(instanceIdx + 1, linePointsCount - 1);
   uint nextNextInstance = min(instanceIdx + 2, linePointsCount - 1);

   vec2 dirNext = normalize(vertex[nextInstance] - vertex[nextNextInstance]);
   if (nextInstance == nextNextInstance)
   {
      dirNext = dir;
   }
   vec2 perpNext = vec2(-dirNext.y, dirNext.x) * (lineWidth * 0.5);

   vec2 pos;

   // Triangle strip order: vertex[idx] + perp, vertex[idx] - perp, vertex[idx + 1] + perpNext, vertex[idx + 1] - perpNext
   // 
   // 0 ---- 2
   // |    / |
   // |  /   |
   // 1 ---- 3
   // 
   // (0, 1, 2) | (2, 1, 3)

   if (vertexIdx == 0)
   {
      pos = vertex[instanceIdx] + perp;
   }
   else if (vertexIdx == 1)
   {
      pos = vertex[instanceIdx] - perp;
   }
   else if (vertexIdx == 2)
   {
      pos = vertex[instanceIdx + 1] + perpNext;
   }
   else if (vertexIdx == 3)
   {
      pos = vertex[instanceIdx + 1] - perpNext;
   }

   gl_Position = projection * vec4(pos, 0.0, 1.0);
}