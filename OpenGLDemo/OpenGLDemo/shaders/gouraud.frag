#version 330 core

in vec2 UV;
in vec3 vWorldSpaceFragment;
in vec3 vWorldSpaceNormal;
in vec3 FragColor; // Interpolated color from the vertex shader

out vec4 FinalColor;

void main() {
    FinalColor = vec4(FragColor, 1.0f);
}
