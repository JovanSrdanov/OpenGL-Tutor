#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

out vec3 FragColor;

struct DirectionalLight {
    vec3 Position;
    vec3 Direction;
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
};

struct Material {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shininess;
};

uniform DirectionalLight uDirLight;
uniform Material uMaterial;

void main() {
    vec3 WorldSpaceVertex = vec3(uModel * vec4(aPos, 1.0f));
    vec3 WorldSpaceNormal = normalize(mat3(transpose(inverse(uModel))) * aNormal);

    vec3 DirLightVector = normalize(-uDirLight.Direction);
    float DirDiffuse = max(dot(WorldSpaceNormal, DirLightVector), 0.0f);
    vec3 DirAmbientColor = uDirLight.Ka * uMaterial.Ka;
    vec3 DirDiffuseColor = DirDiffuse * uDirLight.Kd * uMaterial.Kd;

    FragColor = DirAmbientColor + DirDiffuseColor;

    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
}
