#version 330 core

layout(location = 0) in vec3 vertexNormal_modelspace;
layout(location = 1) in vec3 vertexPosition_modelspace;


// Output data ; will be interpolated for each fragment.
out float distance;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 normalMatrix;
uniform vec3 LightPosition_worldspace;


void main()
{
    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
    EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;
    vec3 vertex2Light_cameraspace = LightPosition_cameraspace - vertexPosition_cameraspace;

    LightDirection_cameraspace = normalize(vertex2Light_cameraspace);
    distance = length(vertex2Light_cameraspace);

    // Normal of the the vertex, in camera space
    Normal_cameraspace = ( normalMatrix * vec4(vertexNormal_modelspace,0)).xyz;
}
