layout (location = 0) in vec3 vertex;
layout (location = 2) in vec2 in_uv;

out vec2 uvCoords;

layout( std140 ) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 transform;

void main()
{	
    uvCoords = in_uv;
	
    gl_Position = projection * transform * vec4(vertex.xy, 0.0, 1.0);
}  