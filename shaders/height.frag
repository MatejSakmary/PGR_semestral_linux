#version 330 core

in vec3 mycol;
in vec2 texCoord;
in vec3 fragPos;

out vec4 FragColor;

uniform sampler2D texture_normal1;
uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;

void main()
{

	// swap z with y because texture is in tangent space thus (0,0,1)
	// means up, but i want (0, 1, 0) to be up, because the terrain is
	// rotated by 90 degrees along X axis, and I don't want to compute
	// tangent space
	vec3 normal = normalize(texture(texture_normal1, texCoord).xzy);
	vec3 lightDir = normalize(lightPos - fragPos);

    vec4 texColor = texture(texture_diffuse1, texCoord);
	float diff = max(dot(normal, lightDir), 0.0);
	vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0) * texColor;
	vec4 diffuse = diff * texColor;
	vec4 result = (ambient + diffuse);
	FragColor = result;
}