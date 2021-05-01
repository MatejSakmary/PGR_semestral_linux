#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 PVMmatrix;
uniform mat4 Model;
uniform mat4 NormalModel;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 texCoords;

void main() 
{
  texCoords = aTexCoords;
  fragPos = vec3(Model * vec4(aPosition, 1.0f));
  fragNormal = vec3(NormalModel * vec4(aNormal,0.0f));

  gl_Position = PVMmatrix * vec4(aPosition, 1.0f);
}