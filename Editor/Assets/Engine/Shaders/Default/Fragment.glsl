#version 420 core

layout( location = 0 ) out vec4 out_color;
layout( location = 1 ) out vec4 out_normal;

struct MaterialSamplerData
{
	sampler2D diffuse;
	sampler2D specular;
};

struct Light_t {
	vec3 position;
	vec3 rotation;
	vec3 color;
	vec3 attenuation;
};

layout( std140 ) uniform Lights {
	Light_t lights[ 4 ];
};

layout( std140 ) uniform Material {
	vec3 diffuseColor;
	vec3 specularColor;
	vec3 ambientColor;
	float shininiess;
	float textureScale;
}material;

uniform MaterialSamplerData materialSamplers;

uniform samplerCube envMap;

in vec2 uv;
in vec3 normalDirection;
in vec3 worldPos;
in vec3 cameraDirection;

vec3 CalculateBaseLightning( vec3 lightDirection, int lightNr ) {
	vec3 normal = normalize( normalDirection );

	float dirDot = dot( normal, lightDirection );
	float brightness = max( dirDot, 0.0f );

	vec3 halfwayDir = normalize( lightDirection + cameraDirection );
	float dampedFactor = pow( max( dot( normal, halfwayDir ), 0.0 ), material.shininiess );

	vec3 ambient = ( ( lights[ lightNr ].color * material.ambientColor ) * texture( materialSamplers.diffuse, uv * material.textureScale ).xyz );
	vec3 diffuse = ( ( lights[ lightNr ].color * material.diffuseColor ) * texture( materialSamplers.diffuse, uv * material.textureScale ).xyz ) * brightness;
	vec3 specular = ( material.specularColor * dampedFactor * lights[ lightNr ].color ) * texture( materialSamplers.specular, uv ).xyz;

	return diffuse + specular + ambient;
}

vec3 CalculateDirectionalLight( int lightNr ) {
	vec3 lightDirection = normalize( lights[ lightNr ].position - worldPos.xyz );

	return CalculateBaseLightning( lightDirection, lightNr );
}

vec3 CalculatePointLight( int lightNr ) {
	vec3 lightDirection = normalize( lights[ lightNr ].position - worldPos.xyz );
	vec3 color = CalculateBaseLightning( lightDirection, lightNr );

	float distance = length( lights[ lightNr ].position - worldPos );
	float attenuation = 1.0 / ( lights[ lightNr ].attenuation[ 0 ] + lights[ lightNr ].attenuation[ 1 ] * distance +
		lights[ lightNr ].attenuation[ 2 ] * ( distance * distance ) );

	attenuation = max( attenuation, 0.f );
	attenuation = min( attenuation, 1.f );

	color *= attenuation;
	color *= lights[ lightNr ].color;

	return color;
}

void main( void ) {
	vec4 directionalLight = vec4( CalculateDirectionalLight( 0 ), 1.0f );

	vec4 pointLight1 = vec4( CalculatePointLight( 1 ), 1.0f );
	vec4 pointLight2 = vec4( CalculatePointLight( 2 ), 1.0f );

	out_color = ( directionalLight + pointLight1 + pointLight2 );
	out_normal = vec4(normalDirection, 1.f);
}