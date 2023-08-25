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
	sampler2D Kd;
	sampler2D Ks;
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

uniform int uIsDrawingLines;
uniform vec3 uLineColor;

void main() {
	 if (uIsDrawingLines == 1) {
        FragColor = vec4(uLineColor, 1.0f);
		return;
    }

	vec3 ViewDirection = normalize(uViewPos - vWorldSpaceFragment);

	// Directional Light
	vec3 DirLightVector = normalize(-uDirLight.Direction);
	float DirDiffuse = max(dot(vWorldSpaceNormal, DirLightVector), 0.0f);
	vec3 DirReflectDirection = reflect(-DirLightVector, vWorldSpaceNormal);
	float DirSpecular = pow(max(dot(ViewDirection, DirReflectDirection), 0.0f), uMaterial.Shininess);
	vec3 DirAmbientColor = uDirLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 DirDiffuseColor = uDirLight.Kd * DirDiffuse * vec3(texture(uMaterial.Kd, UV));
	vec3 DirSpecularColor = uDirLight.Ks * DirSpecular * vec3(texture(uMaterial.Ks, UV));
	vec3 DirColor = DirAmbientColor + DirDiffuseColor + DirSpecularColor;

	// Sun
	vec3 PtLightVector = normalize(uSunLight.Position - vWorldSpaceFragment);
	float PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	vec3 PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	float PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 PtAmbientColor = uSunLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 PtDiffuseColor = PtDiffuse * uSunLight.Kd * vec3(texture(uMaterial.Kd, UV));
	vec3 PtSpecularColor = PtSpecular * uSunLight.Ks * vec3(texture(uMaterial.Ks, UV));

	float PtLightDistance = length(uSunLight.Position - vWorldSpaceFragment);
	float PtAttenuation = 1.0f / (uSunLight.Kc + uSunLight.Kl * PtLightDistance + uSunLight.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColorSun = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);


	// FlashLight
	vec3 SpotlightVector3 = normalize(uFlashLight.Position - vWorldSpaceFragment);

	float SpotDiffuse3 = max(dot(vWorldSpaceNormal, SpotlightVector3), 0.0f);
	vec3 SpotReflectDirection3 = reflect(-SpotlightVector3, vWorldSpaceNormal);
	float SpotSpecular3 = pow(max(dot(ViewDirection, SpotReflectDirection3), 0.0f), uMaterial.Shininess);

	vec3 SpotAmbientColor3 = uFlashLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 SpotDiffuseColor3 = SpotDiffuse3 * uFlashLight.Kd * vec3(texture(uMaterial.Kd, UV));
	vec3 SpotSpecularColor3 = SpotSpecular3 * uFlashLight.Ks * vec3(texture(uMaterial.Ks, UV));

	float SpotlightDistance3 = length(uFlashLight.Position - vWorldSpaceFragment);
	float SpotAttenuation3 = 1.0f / (uFlashLight.Kc + uFlashLight.Kl * SpotlightDistance3 + uFlashLight.Kq * (SpotlightDistance3 * SpotlightDistance3));

	float Theta3 = dot(SpotlightVector3, normalize(-uFlashLight.Direction));
	float Epsilon3 = uFlashLight.InnerCutOff - uFlashLight.OuterCutOff;
	float SpotIntensity3 = clamp((Theta3 - uFlashLight.OuterCutOff) / Epsilon3, 0.0f, 1.0f);
	vec3 SpotColor3 = SpotIntensity3 * SpotAttenuation3 * (SpotAmbientColor3 + SpotDiffuseColor3 + SpotSpecularColor3);

	vec3 FinalColor = DirColor + PtColorSun+SpotColor3;
	FragColor = vec4(FinalColor, 1.0f);
}