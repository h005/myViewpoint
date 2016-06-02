
#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec4 vColor;

// 输出类型要与GLOffscreenRenderFramework创建的buffer一致
out vec4 rgba;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){
    vec3 color = texture2D( myTextureSampler, UV ).rgb + vColor.rgb;
    rgba = vec4(color, 1.f);
}
