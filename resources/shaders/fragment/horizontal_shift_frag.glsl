#version 330 core
uniform sampler2D Texture0;
out vec4 color;
in vec2 texCoord;

void main() {
  	vec4 texColor0 = texture(Texture0, texCoord);

    color = vec4(texColor0.rgb, 1);
}

