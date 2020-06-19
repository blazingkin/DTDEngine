#version 330 core

in vec2 texCoord;
out vec4 color;
uniform sampler2D Texture0;



void main(){
   vec4 tColor = texture( Texture0, texCoord );
   color = tColor;
}