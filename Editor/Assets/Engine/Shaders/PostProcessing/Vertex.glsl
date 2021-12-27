layout (location = 0) in vec3 vertex;
layout (location = 2) in vec2 in_uv;

out vec2 uvCoords;
out vec2 viewRay;

uniform float aspectRatio;
uniform float tanHalfFOV;

void main()
{	
    uvCoords = in_uv;
    gl_Position = vec4(vertex, 1.0);

    viewRay.x = vertex.x * aspectRatio * tanHalfFOV;
    viewRay.y = vertex.y * tanHalfFOV;
}  