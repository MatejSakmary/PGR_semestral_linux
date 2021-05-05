#version 330 core

layout (location = 0) in vec3 aPosition;

uniform mat4 PVMmatrix;
uniform mat4 Model;
uniform ivec2 HALF_TERRAIN_SIZE;
uniform sampler2D heightMapTexture;
uniform float scale;
uniform float half_scale;

out vec3 fragPos;
out vec3 mycol;
out vec2 texCoord;

void main() 
{
    float height = texture(heightMapTexture, aPosition.xz).r*scale - half_scale;
    vec2 pos = ((aPosition.xz * 2.0) - 1) * HALF_TERRAIN_SIZE;

    fragPos = vec3(Model * vec4(aPosition,1.0f));
    mycol = vec3(height, height, height);
    texCoord = vec2(aPosition.xz);
    gl_Position = PVMmatrix * vec4(aPosition.x, height, aPosition.z, 1);
}