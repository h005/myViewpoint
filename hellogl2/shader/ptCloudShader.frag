#version 330 core

// Interpolated values from the vertex shaders
in vec3 vColor;

// Ouput data
layout(location=0) out vec4 color;

void main(){
    color = vec4(vColor, 1);
}
