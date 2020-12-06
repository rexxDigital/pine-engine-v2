#version 330 core

out vec4 outputColor;

in vec2 uvCoords;
in vec2 viewRay;

uniform sampler2D fragColor;
uniform sampler2D fragNormal;
uniform sampler2D fragDepth;
uniform sampler2D noiseTexture;

uniform vec3 samples[64];

layout( std140 ) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

void main()
{
    outputColor = texture(fragColor, uvCoords);
}