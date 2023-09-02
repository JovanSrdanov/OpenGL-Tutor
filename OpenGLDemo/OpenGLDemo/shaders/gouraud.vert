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
out vec3 FragColor; // Store the color per vertex here

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

    // Calculate lighting per vertex
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

    vec3 SpotlightVector3 = normalize(uFlashLight.Position - WorldSpaceVertex);
    float SpotDiffuse3 = max(dot(WorldSpaceNormal, SpotlightVector3), 0.0f);
    vec3 SpotReflectDirection3 = reflect(-SpotlightVector3, WorldSpaceNormal);
    float SpotSpecular3 = pow(max(dot(ViewDirection, SpotReflectDirection3), 0.0f), uMaterial.Shininess);
    vec3 SpotAmbientColor3 = uFlashLight.Ka * uMaterial.Ka;
    vec3 SpotDiffuseColor3 = SpotDiffuse3 * uFlashLight.Kd * uMaterial.Kd;
    vec3 SpotSpecularColor3 = SpotSpecular3 * uFlashLight.Ks * uMaterial.Ks;
    float SpotlightDistance3 = length(uFlashLight.Position - WorldSpaceVertex);
    float SpotAttenuation3 = 1.0f / (uFlashLight.Kc + uFlashLight.Kl * SpotlightDistance3 + uFlashLight.Kq * (SpotlightDistance3 * SpotlightDistance3));
    float Theta3 = dot(SpotlightVector3, normalize(-uFlashLight.Direction));
    float Epsilon3 = uFlashLight.InnerCutOff - uFlashLight.OuterCutOff;
    float SpotIntensity3 = clamp((Theta3 - uFlashLight.OuterCutOff) / Epsilon3, 0.0f, 1.0f);
    vec3 SpotColor3 = SpotIntensity3 * SpotAttenuation3 * (SpotAmbientColor3 + SpotDiffuseColor3 + SpotSpecularColor3);

    // Sum up the vertex colors
   // FragColor = vec3(DirColor + PtColorSun + SpotColor3);
    FragColor = DirColor +PtColorSun+SpotColor3;

    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
}
