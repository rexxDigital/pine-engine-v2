#version 330 core

out vec4 outputColor;

in vec2 uvCoords;
in vec2 viewRay;

uniform sampler2D fragColor;

uniform vec3 samples[64];

layout( std140 ) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

void main()
{
	// We need to flip the texture so it's the right side up for the screen
	vec2 coords = uvCoords;
	coords.y = 1 - coords.y;

    outputColor = texture(fragColor, coords);
}