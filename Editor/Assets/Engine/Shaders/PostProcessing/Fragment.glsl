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
    outputColor = texture(fragColor, uvCoords);
	outputColor = vec4(1.f, 1.f, 1.f, 1.f);
}