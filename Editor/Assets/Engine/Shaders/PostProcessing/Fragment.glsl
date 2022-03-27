out vec4 outputColor;

in vec2 uvCoords;
in vec2 viewRay;

uniform sampler2D fragColor;

uniform vec3 samples[64];

uniform vec2 viewport;

layout( std140 ) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

void main()
{
    outputColor = texture(fragColor, vec2(uvCoords.x, 1 - uvCoords.y) * viewport);
}