#version 330 core

layout (location = 0) in vec3 aPosition;
// layout (location = 1) in vec3 aNormal;
// layout (location = 2) in vec2 aTexCoords;

uniform mat4 PVMmatrix;
uniform ivec2 HALF_TERRAIN_SIZE;
uniform sampler2D heightMapTexture;
uniform float scale;
uniform float half_scale;

out vec3 mycol;
out vec3 fragPos;
out vec3 fragNormal;
out vec2 texCoords;

void main() 
{
    float height = texture(heightMapTexture, aPosition.xz).r*scale - half_scale;
    vec2 pos = ((aPosition.xz * 2.0) - 1) * HALF_TERRAIN_SIZE;

    mycol = vec3(height, height, height);
    gl_Position = PVMmatrix * vec4(aPosition.x, height, aPosition.z, 1);
    // gl_Position = PVMmatrix * vec4(aPosition, 1);
}