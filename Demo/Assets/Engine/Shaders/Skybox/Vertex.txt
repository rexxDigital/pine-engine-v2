#version 330 core

layout (location = 0) in vec3 vertex;

out vec3 uvCoords;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
	mat4 fixedView = mat4(mat3(view));
	
    uvCoords = vertex;
	vec4 pos = projection * fixedView * vec4(vertex, 1.0);
    gl_Position = pos.xyww;
}  