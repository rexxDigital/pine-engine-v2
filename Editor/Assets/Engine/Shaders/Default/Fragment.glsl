layout( location = 0 ) out vec4 out_color;

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
	float cutOffAngle;
	float cutOffSmoothness;
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

	vec3 ambient = ( ( lights[ lightNr ].color * ( material.diffuseColor * material.ambientColor ) ) * texture( materialSamplers.diffuse, uv * material.textureScale ).xyz );
	vec3 diffuse = ( ( lights[ lightNr ].color * material.diffuseColor ) * texture( materialSamplers.diffuse, uv * material.textureScale ).xyz ) * brightness;
	vec3 specular = ( material.specularColor * dampedFactor * lights[ lightNr ].color ) * texture( materialSamplers.specular, uv ).xyz;

	return diffuse + specular + ambient;
}

vec3 CalculateDirectionalLight( int lightNr ) {
	vec3 lightDirection = normalize( lights[ lightNr ].rotation );

	return CalculateBaseLightning( lightDirection, lightNr );
}

vec3 CalculatePointLight( int lightNr ) {
	vec3 lightDirection = normalize( lights[ lightNr ].position - worldPos.xyz );
	vec3 color = CalculateBaseLightning( lightDirection, lightNr );

	return lights[ lightNr ].rotation;

	float distance = length( lights[ lightNr ].position - worldPos );
	float attenuation = 1.0 / ( lights[ lightNr ].attenuation[ 0 ] + lights[ lightNr ].attenuation[ 1 ] * distance +
		lights[ lightNr ].attenuation[ 2 ] * ( distance * distance ) );

	float theta = dot( lightDirection, normalize( -lights[ lightNr ].rotation ) );
	float cutOffAng = lights[ lightNr ].cutOffAngle;

	attenuation = max( attenuation, 0.f );
	attenuation = min( attenuation, 1.f );

//	color *= max( sign( theta - cutOffAng ), 0.0 );
	color *= attenuation;
	color *= lights[ lightNr ].color;

	return color;
}

void main( void ) {
	// calculate transparency
	#if defined(TRANSPARENCY_DISCARD)
		if ( texture( materialSamplers.diffuse, uv * material.textureScale ).a < 0.1 )
			discard;
	#endif

	// calculate the main directional light's lightning
	vec4 directionalLight = vec4( CalculateDirectionalLight( 0 ), 1.0f );

	// calculate point light's lightning

	// TODO: change this to a loop depending on the current amount of
	// dynamic lights.

#if defined(PERFORMACE_FAST) // Just avoid point lights all together lol
	vec4 pointLight1 = vec4( 0.0f );
	vec4 pointLight2 = vec4( 0.0f );
#else
	vec4 pointLight1 = vec4( CalculatePointLight( 1 ), 1.0f );
	vec4 pointLight2 = vec4( CalculatePointLight( 2 ), 1.0f );
#endif

	// add it all up to the final fragment color
	out_color = ( directionalLight + pointLight1 + pointLight2 );
}