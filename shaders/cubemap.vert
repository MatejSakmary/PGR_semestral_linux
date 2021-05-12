#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;
out vec3 fragPosition;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 rotation;

void main()
{
    TexCoords = vec3(rotation * vec4(aPos,1.0));
    fragPosition = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(aPos, 1.0);
}