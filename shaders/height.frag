#version 330 core

in vec3 fragNormal;
in vec3 fragPos;

void main()
{
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0);
	vec4 diffuse = diff * vec4(0.5, 0.5, 0.5, 1.0);
	vec4 result = (ambient + diffuse);
	color = vec4(result);
}