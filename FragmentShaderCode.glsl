#version 430

in vec2 UV;
in vec3 normalspace;
in vec3 positionspace;

uniform sampler2D myTextureSampler0;
uniform sampler2D myTextureSampler1;
uniform vec3 ambientLight;
uniform vec3 ambientLight1;
uniform vec3 lightPosition;
uniform vec3 lightPosition1;
uniform vec3 diffBrightness;
uniform vec3 diffBrightness1;
uniform vec3 eyePosition;
uniform int specBrightness;
uniform int specBrightness1;
uniform bool normalMapping_flag;

out vec4 dacolor;

void main()
{
	//vec3 color = texture( myTextureSampler0, UV ).rgb;
	//finalcolor = (0.7*texture(myTextureSampler0, UV )+0.3*texture(myTextureSampler1, UV )).rgb;
	
	vec3 MaterialAmbientColor = texture(myTextureSampler0, UV).rgb;
	vec3 MaterialDiffuseColor = texture(myTextureSampler0, UV).rgb;
	vec3 MaterialSpecularColor = vec3(1.0,1.0,1.0);
	
	vec3 MaterialAmbientColor1 = texture(myTextureSampler0, UV).rgb;
	vec3 MaterialDiffuseColor1 = texture(myTextureSampler0, UV).rgb;
	vec3 MaterialSpecularColor1 = vec3(1.0,1.0,1.0);

	vec3 normal = normalize(normalspace);
	
	if(normalMapping_flag)
	{
		normal = texture(myTextureSampler1, UV ).rgb;
		//normal =  0.7 * texture(myTextureSampler0, UV).rgb
		//+ 0.3 * texture(myTextureSampler1, UV).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		//vec3 MaterialAmbientColor = 0.7 * texture(myTextureSampler0, UV).rgb
		//+ 0.3 * texture(myTextureSampler1, UV).rgb;
		//vec3 MaterialDiffuseColor =  0.7 * texture(myTextureSampler0, UV).rgb
		//+ 0.3 * texture(myTextureSampler1, UV).rgb;
	}
	

	vec3 lightVector = normalize(lightPosition - positionspace);
	float brightness = dot(lightVector, normalize(normal));
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);

	vec3 lightVector1 = normalize(lightPosition1 - positionspace);
	float brightness1 = dot(lightVector1, normalize(normal));
	vec4 diffuseLight1 = vec4(brightness1, brightness1, brightness1, 1.0);

	vec3 reflectedLightVector = reflect(-lightVector, normal);
	vec3 eyeVector = normalize(eyePosition - positionspace);
	float spec = clamp(dot(reflectedLightVector, eyeVector), 0, 1);
	//vec4 specularLight = vec4(pow(spec, specBrightness), 0, 0, 1);
	vec4 specularLight = vec4(pow(spec, specBrightness), pow(spec, specBrightness), pow(spec, specBrightness), 1);
	//vec4 specularLight1 = vec4(pow(spec, specBrightness1), pow(spec, specBrightness), pow(spec, specBrightness), 1);
	vec4 specularLight1 = vec4(pow(spec, specBrightness), pow(spec, specBrightness), pow(spec, specBrightness), 1);
    
	dacolor =0.3*(vec4(MaterialAmbientColor, 1.0) * vec4(ambientLight, 1.0) + 
	vec4(MaterialDiffuseColor, 1.0) * clamp(diffuseLight, 0, 1) * vec4(diffBrightness, 1.0) +
	vec4(MaterialSpecularColor, 1.0) * specularLight)+
	vec4(MaterialAmbientColor, 1.0) * vec4(ambientLight1, 1.0) + 
	vec4(MaterialDiffuseColor, 1.0) * clamp(diffuseLight1, 0, 1) * vec4(diffBrightness1, 1.0) +
	vec4(MaterialSpecularColor1, 1.0) * specularLight1;

	//dacolor=vec4(texture( myTextureSampler0, UV ).rgb,0);
}