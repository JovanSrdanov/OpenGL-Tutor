#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

out vec2 UV;
out vec3 vWorldSpaceFragment;
out vec3 vWorldSpaceNormal;
out vec3 FragColor; 

struct PositionalLight {
    vec3 Position;
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Kc;
    float Kl;
    float Kq;
};

struct DirectionalLight {
    vec3 Position;
    vec3 Direction;
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float InnerCutOff;
    float OuterCutOff;
    float Kc;
    float Kl;
    float Kq;
};

struct Material {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shininess;
};

uniform PositionalLight uSunLight;
uniform DirectionalLight uFlashLight;
uniform DirectionalLight uDirLight;
uniform Material uMaterial;
uniform vec3 uViewPos;

void main() {
 	vec3 WorldSpaceVertex = vec3(uModel * vec4(aPos, 1.0f));
	vec3 WorldSpaceNormal = normalize(mat3(transpose(inverse(uModel))) * aNormal);
	vec3 ViewDirection = normalize(uViewPos - WorldSpaceVertex);

    vec3 DirLightVector = normalize(-uDirLight.Direction);
    float DirDiffuse = max(dot(WorldSpaceNormal, DirLightVector), 0.0f);
    vec3 DirReflectDirection = reflect(-DirLightVector, WorldSpaceNormal);
    float DirSpecular = pow(max(dot(ViewDirection, DirReflectDirection), 0.0f), uMaterial.Shininess);
    vec3 DirAmbientColor = uDirLight.Ka * uMaterial.Ka;
    vec3 DirDiffuseColor = uDirLight.Kd * DirDiffuse * uMaterial.Kd;
    vec3 DirSpecularColor = uDirLight.Ks * DirSpecular * uMaterial.Ks;
    vec3 DirColor = DirAmbientColor + DirDiffuseColor + DirSpecularColor;

    vec3 PtLightVector = normalize(uSunLight.Position - WorldSpaceVertex);
    float PtDiffuse = max(dot(WorldSpaceNormal, PtLightVector), 0.0f);
    vec3 PtReflectDirection = reflect(-PtLightVector, WorldSpaceNormal);
    float PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);
    vec3 PtAmbientColor = uSunLight.Ka * uMaterial.Ka;
    vec3 PtDiffuseColor = PtDiffuse * uSunLight.Kd * uMaterial.Kd;
    vec3 PtSpecularColor = PtSpecular * uSunLight.Ks * uMaterial.Ks;
    float PtLightDistance = length(uSunLight.Position - WorldSpaceVertex);
    float PtAttenuation = 1.0f / (uSunLight.Kc + uSunLight.Kl * PtLightDistance + uSunLight.Kq * (PtLightDistance * PtLightDistance));
    vec3 PtColorSun = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);

    vec3 SpotlightVector = normalize(uFlashLight.Position - WorldSpaceVertex);
    float SpotDiffuse = max(dot(WorldSpaceNormal, SpotlightVector), 0.0f);
    vec3 SpotReflectDirection = reflect(-SpotlightVector, WorldSpaceNormal);
    float SpotSpecular = pow(max(dot(ViewDirection, SpotReflectDirection), 0.0f), uMaterial.Shininess);
    vec3 SpotAmbientColor = uFlashLight.Ka * uMaterial.Ka;
    vec3 SpotDiffuseColor = SpotDiffuse * uFlashLight.Kd * uMaterial.Kd;
    vec3 SpotSpecularColor = SpotSpecular * uFlashLight.Ks * uMaterial.Ks;
    float SpotlightDistance = length(uFlashLight.Position - WorldSpaceVertex);
    float SpotAttenuation = 1.0f / (uFlashLight.Kc + uFlashLight.Kl * SpotlightDistance + uFlashLight.Kq * (SpotlightDistance * SpotlightDistance));
    float Theta = dot(SpotlightVector, normalize(-uFlashLight.Direction));
    float Epsilon = uFlashLight.InnerCutOff - uFlashLight.OuterCutOff;
    float SpotIntensity = clamp((Theta - uFlashLight.OuterCutOff) / Epsilon, 0.0f, 1.0f);
    vec3 SpotColor = SpotIntensity * SpotAttenuation * (SpotAmbientColor + SpotDiffuseColor + SpotSpecularColor);

    FragColor = DirColor +PtColorSun+SpotColor;
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
}
