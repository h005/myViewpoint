#version 330 core

// Interpolated values from the vertex shaders
in vec3 vColor;

// Ouput data
out vec3 color;

void main(){
    color = vColor;
}
