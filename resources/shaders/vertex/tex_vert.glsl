#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform mat4 LS;
uniform vec3 lightPos;
uniform vec3 viewVector;

out vec2 vTexCoord;
out vec3 fragNor;
out vec3 lightDir;
out vec3 viewVectorModel;
out vec4 fPosLS;

void main() {


  /* First model transforms */
  gl_Position = P * V * M * vec4(vertPos.xyz, 1.0);
  vec3 worldVPos = (M * vec4(vertPos.xyz, 1.0)).xyz;

  fragNor = (M * vec4(vertNor, 0.0)).xyz;
  /* diffuse coefficient for a directional light */
  /* pass through the texture coordinates to be interpolated */
  vTexCoord = vertTex;
  fPosLS = LS * M * vec4(vertPos.xyz, 1.0);
  /* Calculate light and view vectors used for lighting */
  viewVectorModel = normalize(viewVector - worldVPos);
	lightDir = normalize(lightPos - worldVPos);
}
