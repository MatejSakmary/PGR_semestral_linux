#version 330 core

in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D fireTex;

void main(){
    vec4 color = texture(fireTex, vec2(texCoords.x, 1-texCoords.y));
    if(color.a < 0.2){
        discard;
    }
    FragColor = color;
//    FragColor = vec4(texCoords, 0.0f, 1.0f);
}
