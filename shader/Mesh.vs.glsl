#version 330 core

#include "LayoutLocations.glsl"

layout (location = VERTICES_LAYOUT_LOCATION) in vec3 position;
layout (location = NORMALS_LAYOUT_LOCATION) in vec3 normal;

out VertexData
{
    vec3 position;
    vec3 normal;
} outData;

#include "UniformMatrices.glsl"

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    outData.position = position;
    outData.normal = normalize(normal);
}
