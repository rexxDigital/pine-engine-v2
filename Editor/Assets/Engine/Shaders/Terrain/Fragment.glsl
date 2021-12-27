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
};

layout( std140 ) uniform Lights {
	Light_t lights[ 4 ];
};

in vec2 uv;
in vec3 normalDirection;
in vec3 worldPos;
in vec3 cameraDirection;

void main( void ) {
	out_color = vec4(1.f, 1.f, 1.f, 1.f);
}