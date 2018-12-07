#version 330 core

out vec4 fragColour;

uniform int renderMode;

void main()
{
    const vec3 vertexColour = vec3(1.0, 0.0, 0.0);
    const vec3 lineColour = vec3(0.0, 0.015, 0.375);
    fragColour = vec4(vertexColour, 1.0);
//    if (renderMode == 0)
//    {
//        fragColour = vec4(vertexColour, 1.0);
//    }
//    else if (renderMode == 1)
//    {
//        fragColour = vec4(lineColour, 1.0);
//    }
}
