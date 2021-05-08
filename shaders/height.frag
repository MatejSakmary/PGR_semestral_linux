#version 330 core

struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
	float shininess;
};

in vec3 mycol;
in vec2 texCoord;
in vec3 fragPos;

out vec4 FragColor;

uniform Material material;
uniform vec3 lightPos;

void main()
{
	vec3 lightDir = normalize(lightPos - fragPos);

    vec4 texColor = texture(material.texture_diffuse1, texCoord);
	float diff = max(dot(normal, lightDir), 0.0);
	vec4 ambient = vec4(0.20, 0.20, 0.20, 1.0) * texColor;
	vec4 diffuse = diff * texColor * vec4(0.08, 0.08, 0.15 ,1);
	vec4 result = (ambient + diffuse);
	FragColor = result;
}