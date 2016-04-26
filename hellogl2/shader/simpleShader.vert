#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location=0) in vec2 vertexUV;
layout(location=1) in vec3 vertexPosition_modelspace;
layout(location=2) in vec3 vertexNormal_modelspace;
layout(location=3) in vec3 vertexColor;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 vColor;

uniform mat4 projMatrix;
uniform mat4 mvMatrix;
uniform mat4 normalMatrix;
uniform vec4 diffuse;

void main()
{
	vec4 viewSpacePos = mvMatrix * vec4(vertexPosition_modelspace, 1);
	gl_Position = projMatrix * viewSpacePos;
    UV = vertexUV;
    vColor = diffuse;
}
