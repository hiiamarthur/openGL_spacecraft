#version 430

in vec3 TextCoord;
in vec3 Position;

uniform mat4 modelMatrix;
uniform samplerCube skybox;
out vec4 color;

void main()
{
	vec3 sampleVector = normalize(modelMatrix * vec4(Position, 1.0)).xyz;
	color = textureCube(skybox, sampleVector);
	//color = texture(skybox, TextCoord);
}