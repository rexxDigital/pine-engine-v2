#version 420 core

struct MaterialSamplerData
{
	sampler2D diffuse;
	sampler2D specular;
};

struct Light_t {
	vec3 position;
	vec3 color;
};

layout(std140) uniform Lights {
	Light_t lights[10];
};

layout(std140) uniform Material {
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 ambientColor;
	float shininiess;
}material;

uniform MaterialSamplerData materialSamplers;
uniform samplerCube skybox;

out vec4 outputColor;

in vec2 uv;
in vec3 normalDirection;
in vec3 worldPos;
in vec3 cameraDirection;

vec3 CalculateDirectionalLight(int lightNr) {
	vec3 lightDirection = normalize(lights[lightNr].position);

	float dirDot = dot(normalDirection, lightDirection);
	float brightness = max(dirDot, 0.0f);

	vec3 lightDir = -lightDirection;
	vec3 reflectedLightDirection = reflect(lightDirection, normalDirection);

	float specularFactor = max(dot(reflectedLightDirection, lightDir), 0.0f);
	float dampedFactor = pow(specularFactor, material.shininiess);
	
	vec3 ambient = lights[lightNr].color * material.ambientColor;
	vec3 diffuse = (lights[lightNr].color * material.diffuseColor) * brightness;
	vec3 specular = (material.specularColor * dampedFactor) * vec3(texture(materialSamplers.specular, uv));

	return ambient + diffuse + specular;
}

void main(void) {
	
  outputColor = vec4(CalculateDirectionalLight(0), 1.0f) * texture(materialSamplers.diffuse, uv);

}