#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec2 vertexUV;
in layout(location=2) vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


out vec2 UV;
out vec3 normalspace;
out vec3 positionspace;

void main()
{
	vec4 v = vec4(position, 1.0);
	vec4 out_position = projectionMatrix * viewMatrix * modelMatrix * v;
	//vec4 out_position = v;
	gl_Position = out_position;
	UV = vertexUV;

	vec4 normal_temp = projectionMatrix * viewMatrix * modelMatrix * vec4(normal, 0);
	normalspace = normal_temp.xyz;
	positionspace = (modelMatrix * v).xyz;
}