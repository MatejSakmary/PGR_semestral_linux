#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
in vec3 fragPosition;

uniform samplerCube nightCubemap;
uniform samplerCube dayCubemap;
uniform float mixVal;
uniform vec3 cameraPosition;
uniform float a;
uniform float b;

vec3 applyFog(vec3 originalColor, float distance, vec3 cameraPosition, vec3 cameraDirection){
    float fogAmount = clamp(a * exp(-cameraPosition.y * b) * (1.0 -exp(-distance * cameraDirection.y * b))/cameraDirection.y,0.0,1.0);
    vec3  fogColor = vec3(0.25, 0.3, 0.35);

//    if (usedLights >= 2){
//        vec3 lightDirection = normalize(lights[1].position - cameraPosition);
//        float lightIntensity = max(dot(cameraDirection, lightDirection), 0.0);
//        fogColor = mix(vec3(0.5, 0.6, 0.7),
//        lights[1].diffuse,
//        pow(lightIntensity, 60));
//    }
    return mix(originalColor, fogColor, fogAmount);
//    return vec3(distance, distance, distance);
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