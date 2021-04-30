#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
  
uniform mat4 PVMmatrix;

out vec3 ourColor; // output a color to the fragment shader

void main()
{
    gl_Position = PVMmatrix * vec4(aPos, 1.0);
    ourColor = vec3(1.0, 0.0, 0.0); // set ourColor to the input color we got from the vertex data
}  