#version 420 core

layout( location = 0 ) in vec3 vertex;
layout( location = 1 ) in vec3 normal;
layout( location = 2 ) in vec2 in_uv;

layout( std140 ) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 transform;

void main( void ) {
	vec4 worldPosition = vec4( vertex, 1.0 );

	gl_Position = projection * view * transform * worldPosition;
}