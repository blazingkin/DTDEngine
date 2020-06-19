#version 330 core
uniform sampler2D Texture0;
uniform mat4 M;
in vec3 fragNor;
in vec3 lightDir;
in vec3 matColor;
in vec3 viewVectorModel;
out vec4 color;
in vec2 vTexCoord;
in float dCo;

void main() {
  	vec4 texColor0 = texture(Texture0, vTexCoord);

	if (texColor0.w < 0.1) {
		discard;
	}
	if (viewVectorModel.y < 0) {
		discard;
	} else {
		color = texColor0;
	}
}

