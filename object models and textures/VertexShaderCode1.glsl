#version 430

layout(location = 0) in vec3 position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 TextCoord;
out vec3 Position;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0); 
	Position = position;
	TextCoord = vec3(100,100,100) * position;
}