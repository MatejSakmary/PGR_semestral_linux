#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 texCoords;

uniform vec3 lightPos;

uniform sampler2D texture_diffuse1;

out vec4 color;

void main()
{
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0) * texture(texture_diffuse1, texCoords);
	vec4 diffuse = diff * texture(texture_diffuse1, texCoords);
	vec4 result = (ambient + diffuse);
	color = vec4(result);
}