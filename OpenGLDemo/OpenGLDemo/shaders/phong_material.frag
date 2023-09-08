#version 330 core

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

in vec2 UV;
in vec3 vWorldSpaceFragment;
in vec3 vWorldSpaceNormal;

out vec4 FragColor;

void main() {
    vec3 ViewDirection = normalize(uViewPos - vWorldSpaceFragment);

    vec3 DirLightVector = normalize(-uDirLight.Direction);
    float DirDiffuse = max(dot(vWorldSpaceNormal, DirLightVector), 0.0f);
    vec3 DirReflectDirection = reflect(-DirLightVector, vWorldSpaceNormal);
    float DirSpecular = pow(max(dot(ViewDirection, DirReflectDirection), 0.0f), uMaterial.Shininess);
    vec3 DirAmbientColor = uDirLight.Ka * uMaterial.Ka;
    vec3 DirDiffuseColor = uDirLight.Kd * DirDiffuse * uMaterial.Kd;
    vec3 DirSpecularColor = uDirLight.Ks * DirSpecular * uMaterial.Ks;
    vec3 DirColor = DirAmbientColor + DirDiffuseColor + DirSpecularColor;

    vec3 PtLightVector = normalize(uSunLight.Position - vWorldSpaceFragment);
    float PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
    vec3 PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
    float PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);
    vec3 PtAmbientColor = uSunLight.Ka * uMaterial.Ka;
    vec3 PtDiffuseColor = PtDiffuse * uSunLight.Kd * uMaterial.Kd;
    vec3 PtSpecularColor = PtSpecular * uSunLight.Ks * uMaterial.Ks;
    float PtLightDistance = length(uSunLight.Position - vWorldSpaceFragment);
    float PtAttenuation = 1.0f / (uSunLight.Kc + uSunLight.Kl * PtLightDistance + uSunLight.Kq * (PtLightDistance * PtLightDistance));
    vec3 PtColorSun = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);

    vec3 SpotlightVector = normalize(uFlashLight.Position - vWorldSpaceFragment);
    float SpotDiffuse = max(dot(vWorldSpaceNormal, SpotlightVector), 0.0f);
    vec3 SpotReflectDirection = reflect(-SpotlightVector, vWorldSpaceNormal);
    float SpotSpecular = pow(max(dot(ViewDirection, SpotReflectDirection), 0.0f), uMaterial.Shininess);
    vec3 SpotAmbientColor = uFlashLight.Ka * uMaterial.Ka;
    vec3 SpotDiffuseColor = SpotDiffuse * uFlashLight.Kd * uMaterial.Kd;
    vec3 SpotSpecularColor = SpotSpecular * uFlashLight.Ks * uMaterial.Ks;
    float SpotlightDistance = length(uFlashLight.Position - vWorldSpaceFragment);
    float SpotAttenuation = 1.0f / (uFlashLight.Kc + uFlashLight.Kl * SpotlightDistance + uFlashLight.Kq * (SpotlightDistance * SpotlightDistance));
    float Theta = dot(SpotlightVector, normalize(-uFlashLight.Direction));
    float Epsilon = uFlashLight.InnerCutOff - uFlashLight.OuterCutOff;
    float SpotIntensity = clamp((Theta - uFlashLight.OuterCutOff) / Epsilon, 0.0f, 1.0f);
    vec3 SpotColor = SpotIntensity * SpotAttenuation * (SpotAmbientColor + SpotDiffuseColor + SpotSpecularColor);

    vec3 FinalColor = DirColor + PtColorSun + SpotColor;
    FragColor = vec4(FinalColor, 1.0f);
}
