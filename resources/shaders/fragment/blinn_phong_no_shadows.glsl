#version 330 core
uniform sampler2D Texture0;
uniform sampler2D normalMap;
uniform vec3 MatDif;
uniform vec3 MatAmb;
uniform vec3 MatSpec;
uniform vec3 lightColor;
uniform mat4 M;
uniform int textureEnabled;
uniform float shine;
in vec3 fragNor;
in vec3 lightDir;
in vec3 matColor;
in vec3 viewVectorModel;
out vec4 color;
in vec2 vTexCoord;
in vec4 fPosLS;

void main() {
  	vec4 texColor0 = texture(Texture0, vTexCoord);

	color = texColor0;
}

