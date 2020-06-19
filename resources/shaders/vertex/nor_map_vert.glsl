#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
layout(location = 3) in vec3 vertTan;
layout(location = 4) in vec3 vertBN;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform mat4 LS;
uniform vec3 lightPos;
uniform vec3 bmLight;
uniform vec3 viewVector;

out vec2 vTexCoord;
out vec3 viewVectorModel;
out vec3 bmLightDir;
out vec4 fPosLS;

void main() {

  mat3 TBN;
  vec3 eyeN = (M*vec4(vertNor, 1.0)).xyz;
  vec3 eyeT = (M*vec4(vertTan, 1.0)).xyz;
  vec3 eyeBN = (M*vec4(vertBN, 1.0)).xyz;

  TBN = transpose(mat3(eyeT, eyeBN, eyeN));

  /* First model transforms */
  gl_Position = P * V * M * vec4(vertPos.xyz, 1.0);
  vec3 worldVPos = (M * vec4(vertPos.xyz, 1.0)).xyz;

  /* diffuse coefficient for a directional light */
  /* pass through the texture coordinates to be interpolated */
  vTexCoord = vertTex;
  fPosLS = LS * M * vec4(vertPos.xyz, 1.0);
  /* Calculate light and view vectors used for lighting */
  viewVectorModel = TBN * normalize(viewVector - worldVPos);
  bmLightDir = TBN * normalize(bmLight);
}
