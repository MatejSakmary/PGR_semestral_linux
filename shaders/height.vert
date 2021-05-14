#version 330 core

layout (location = 0) in vec3 aPosition;

struct Material{
    bool oppacityPresent;

    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
    sampler2D texture_oppacity1;
    float shininess;
};


uniform Material material;
uniform mat4 PVMmatrix;
uniform mat4 Model;
uniform float scale;

out vec3 fragPosition;
out vec3 mycol;
out vec2 texCoords;

void main() 
{
    float height = texture(material.texture_height1, aPosition.xz).r*scale - (scale/2);

    fragPosition = vec3(Model * vec4(aPosition.x, height, aPosition.z,1.0f));
    mycol = vec3(height, height, height);
    texCoords = vec2(aPosition.xz);
    gl_Position = PVMmatrix * vec4(aPosition.x, height, aPosition.z, 1);
}