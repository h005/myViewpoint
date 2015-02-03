#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec3 indice;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

uniform mat4 mvMatrix;
uniform mat4 projMatrix;

void main()
{
	gl_Position = projMatrix * mvMatrix * vec4(vertexPosition_modelspace, 1);
	UV = vertexUV;
}
