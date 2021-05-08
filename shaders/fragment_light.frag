#version 330 core

struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
    float shininess;
};

/* this is a general structure for defining light, some not all attributes are always used
   only the ones required, which parameters shall be used (esentially what type of light
   I have is specified by the type variable which has following meaning:
   0 -> directional light
   1 -> point light
   2 -> spot light
*/
struct Light{
	int type;

	vec3 direction;
	vec3 position;

    vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;

};
#define MAX_NUM_LIGHTS 8

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 texCoords;

uniform vec3 cameraPosition;
uniform Material material;
uniform Light lights[MAX_NUM_LIGHTS];
uniform int usedLights;
uniform bool normalTexUsed;

out vec4 color;

vec3 getDirectionalLight(Light light, vec3 normal, vec3 cameraDirection);
vec3 getPointLight(Light light, vec3 normal, vec3 fragPosition,  vec3 cameraDirection);
vec3 getSpotLight(Light light, vec3 normal, vec3 fragPosition,  vec3 cameraDirection);

void main()
{
	vec3 normal;
	if( normalTexUsed == true){
		// swap z with y because texture is in tangent space thus (0,0,1)
		// means up, but i want (0, 1, 0) to be up, because the terrain is
		// rotated by 90 degrees along X axis, and I don't want to compute
		// tangent space
		normal = (texture(material.texture_normal1, texCoords)).rbg;
		normal = normalize(normal * 2.0 - 1.0);
		// I also must invert the z (previously y) component, sinze openGL
		// reads texture coordinate reversed from how textures are created
		normal = normal * vec3(1.0, 1.0, -1.0);
	} else {
		normal = normalize(fragNormal);
	}
    vec3 cameraDirection = normalize(cameraPosition - fragPosition);

	vec3 result;
	if(usedLights <= 0){
		result = vec3(1.0, 1.0, 1.0);
	}

	for(int i = 0; i < usedLights; i++)
	{
		if(lights[i].type == 0){
			result += getDirectionalLight(lights[i], normal, cameraDirection);
		} else if (lights[i].type == 1){
			result += getPointLight(lights[i], normal, fragPosition, cameraDirection);
		} else if (lights[i].type == 2){
			result += getSpotLight(lights[i], normal, fragPosition, cameraDirection);
		}
	}
	color = vec4(result,1.0);
}

vec3 getDirectionalLight(Light light, vec3 normal, vec3 cameraDirection)
{
	vec3 lightDirection = normalize(-light.direction);
	float diffuse = max(dot(normal, lightDirection), 0.0);

	vec3 reflectedDirection = reflect(-lightDirection, normal);
	float specular = pow(max(dot(cameraDirection, reflectedDirection), 0.0), material.shininess);

	vec3 ambient_light = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
	vec3 diffuse_light = light.diffuse * diffuse * vec3(texture(material.texture_diffuse1, texCoords));
	// Note leaving texture diffuse ONLY FOR NOW, later should be changed to texture_specular or flat color?
	vec3 specular_light = light.specular * specular * vec3(texture(material.texture_diffuse1, texCoords));

	return (ambient_light + diffuse_light + specular_light);
}

vec3 getPointLight(Light light, vec3 normal, vec3 fragPosition,  vec3 cameraDirection)
{
	vec3 lightDirection = normalize(light.position - fragPosition);
	float diffuse = max(dot(normal, lightDirection), 0.0);

	vec3 reflectedDirection = reflect(-lightDirection, normal);
	float specular = pow(max(dot(cameraDirection, reflectedDirection), 0.0), material.shininess);

    float distance = length(light.position - fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient_light = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
	vec3 diffuse_light = light.diffuse * diffuse * vec3(texture(material.texture_diffuse1, texCoords));
	// Note leaving texture diffuse ONLY FOR NOW, later should be changed to texture_specular or flat color?
	vec3 specular_light = light.specular * specular * vec3(texture(material.texture_diffuse1, texCoords));

	ambient_light = ambient_light * attenuation;
	diffuse_light = diffuse_light * attenuation;
	specular_light = specular_light * attenuation;
	return (ambient_light + diffuse_light + specular_light);
}

vec3 getSpotLight(Light light, vec3 normal, vec3 fragPosition,  vec3 cameraDirection)
{
	vec3 lightDirection = normalize(light.position - fragPosition);
	float diffuse = max(dot(normal, lightDirection), 0.0);

	vec3 reflectedDirection = reflect(-lightDirection, normal);
	float specular = pow(max(dot(cameraDirection, reflectedDirection), 0.0), material.shininess);

	float distance = length(light.position - fragPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff)/ epsilon, 0.0, 1.0);

	vec3 ambient_light = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
	vec3 diffuse_light = light.diffuse * diffuse * vec3(texture(material.texture_diffuse1, texCoords));
	// Note leaving texture diffuse ONLY FOR NOW, later should be changed to texture_specular or flat color?
	vec3 specular_light = light.specular * specular * vec3(texture(material.texture_diffuse1, texCoords));

	ambient_light = ambient_light * attenuation * intensity;
	diffuse_light = diffuse_light * attenuation * intensity;
	specular_light = specular_light * attenuation * intensity;
	return (ambient_light + diffuse_light + specular_light);
}
