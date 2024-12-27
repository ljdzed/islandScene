/************************************************
 * Vertex Shader (JOINTS_0 read as float)
 ************************************************/
#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

// Because the second snippet does `glVertexAttribPointer` for JOINTS_0,
// JOINTS_0 are read as float, so we declare vec4 here:
layout(location = 3) in vec4 inJoints;
layout(location = 4) in vec4 inWeights;

uniform mat4 MVP;
uniform mat4 jointMatrices[200];

out vec3 worldNormal;
out vec3 worldPosition;

void main()
{
    // The data inJoints.x, .y, .z, .w might be fractional, so you'd have
    // to round or convert them to int if you still want them as joint indices:
    ivec4 jointIDs = ivec4(inJoints+0.5);
    // Because the second snippet is supplying floats,
    // you might manually convert them to int.

    mat4 skinMat = mat4(0.0);
    skinMat += inWeights[0] * jointMatrices[jointIDs[0]];
    skinMat += inWeights[1] * jointMatrices[jointIDs[1]];
    skinMat += inWeights[2] * jointMatrices[jointIDs[2]];
    skinMat += inWeights[3] * jointMatrices[jointIDs[3]];

    vec4 skinnedPos = skinMat * vec4(inPosition, 1.0);
    gl_Position = MVP * skinnedPos;

    mat3 normalMat = transpose(inverse(mat3(skinMat)));
    worldNormal    = normalize(normalMat * inNormal);

    worldPosition = skinnedPos.xyz;
}
