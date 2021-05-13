#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 PVMmatrix;
uniform int frame;

out vec2 texCoords;

void main()
{
    texCoords = vec2((aTexCoords.x + frame)/200, aTexCoords.y);
    gl_Position = PVMmatrix * vec4(aPosition, 1.0);
}
