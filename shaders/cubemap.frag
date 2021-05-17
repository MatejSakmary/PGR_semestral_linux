#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
in vec3 fragPosition;

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

uniform Light lights[MAX_NUM_LIGHTS];
uniform int usedLights;

uniform samplerCube nightCubemap;
uniform samplerCube dayCubemap;
uniform float mixVal;
uniform vec3 cameraPosition;
uniform float a;
uniform float b;

vec3 applyFog(vec3 originalColor, float distance, vec3 cameraPosition, vec3 cameraDirection){
    float fogAmount = clamp(a * exp(-cameraPosition.y * b) * (1.0 -exp(-distance * cameraDirection.y * b))/cameraDirection.y,0.0,1.0);
    vec3  fogColor = vec3(0.20, 0.25, 0.3);
    if(usedLights >= 2){
        for(int i = 0; i < usedLights; i++){
            if(lights[i].type == 0){
                continue;
            }
            vec3 lightDirection = normalize(lights[i].position - cameraPosition);
            float lightIntensity = max( dot( cameraDirection, lightDirection), 0.0);
            fogColor = mix(fogColor,
            lights[i].diffuse ,
            pow(lightIntensity,400));
        }
    }
    return mix(originalColor, fogColor, fogAmount);
}

void main()
{
    vec3 cameraDirection = normalize(cameraPosition - fragPosition);
    float distance = distance(vec3(0.0,0.0,0.0), fragPosition);
    vec3 color = mix(vec3(texture(dayCubemap, TexCoords)),
                     vec3(texture(nightCubemap, TexCoords)),
                     mixVal);
    color = applyFog(color, distance, cameraPosition, -cameraDirection);
    FragColor = vec4(color,1.0f);
}