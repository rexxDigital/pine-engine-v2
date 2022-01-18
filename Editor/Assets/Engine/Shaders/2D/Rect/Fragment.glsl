out vec4 outputColor;

in vec2 uvCoords;

struct MaterialSamplerData
{
	sampler2D diffuse;
	sampler2D specular;
};

layout( std140 ) uniform Material {
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 ambientColor;
	float shininiess;
	float textureScale;
}material;

uniform MaterialSamplerData materialSamplers;

void main()
{
    outputColor = texture(materialSamplers.diffuse, uvCoords) * vec4(material.diffuseColor, 1);
}