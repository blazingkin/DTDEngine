#version 330 core
out vec4 FragColor;

in vec3 TexCoords;
uniform vec4 lightColor;
uniform samplerCube skybox;

void main() {
  FragColor = texture(skybox, TexCoords) * lightColor;
  //FragColor = vec4(TexCoords, 1.0);
}
