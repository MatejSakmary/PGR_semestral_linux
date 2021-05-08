#version 330 core
struct Material{
	sampler2D texture_normal1;
	sampler2D texture_diffuse1;
	sampler2D texture_height1;
};

in vec3 mycol;
in vec2 texCoord;
in vec3 fragPos;

out vec4 FragColor;

uniform Material material;
uniform vec3 lightPos;

void main()
{

	// swap z with y because texture is in tangent space thus (0,0,1)
	// means up, but i want (0, 1, 0) to be up, because the terrain is
	// rotated by 90 degrees along X axis, and I don't want to compute
	// tangent space
	vec3 normal = (texture(material.texture_normal1, texCoord)).rbg;
	normal = normalize(normal * 2.0 - 1.0);
    // I also must invert the z (previously y) component, sinze openGL
	// reads texture coordinate reversed from how textures are created
	normal = normal * vec3(1.0, 1.0, -1.0);
	vec3 lightDir = normalize(lightPos - fragPos);

    vec4 texColor = texture(material.texture_diffuse1, texCoord);
	float diff = max(dot(normal, lightDir), 0.0);
	vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0) * texColor;
	vec4 diffuse = diff * texColor;
	vec4 result = (ambient + diffuse);
	FragColor = result;
}