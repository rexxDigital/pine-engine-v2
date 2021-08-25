#version 420 core

layout( location = 0 ) in vec3 vertex;
layout( location = 1 ) in vec3 normal;
layout( location = 2 ) in vec2 in_uv;

layout( std140 ) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

out vec2 uv;
out vec3 normalDirection;
out vec3 worldPos;
out vec3 cameraDirection;

uniform mat4 transform;

void main( void ) {
	vec4 worldPosition = vec4( vertex, 1.0 );

	gl_Position = projection * view * transform * worldPosition;

	normalDirection = normalize((transform * vec4(normal, 0.0)).xyz);
	worldPos = ( transform * worldPosition ).xyz;

	cameraDirection = normalize( ( inverse( view ) * vec4( 0.f, 0.f, 0.f, 1.f ) ).xyz - worldPos.xyz );

	uv = in_uv;
}