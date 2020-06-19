#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
layout(location = 3) in vec4 rotationQuat;
layout(location = 4) in vec4 velocity;
layout(location = 5) in mat4 M;
uniform mat4 P;
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

  vec4 vPosition;

  /* Calculate the model rotation based on the quaternion */
  vec3 temp = cross(rotationQuat.xyz, vertPos.xyz) + rotationQuat.w * vertPos.xyz;
  vec3 rotated = vertPos.xyz + 2.0 * cross(rotationQuat.xyz, temp);

  /* First model transforms */


  vec3 worldVPos = (M * vec4(rotated, 1.0)).xyz;

  vec3 tempNor = cross(rotationQuat.xyz, vertNor.xyz) + rotationQuat.w * vertNor.xyz;
  vec3 rotatedNor = vertNor.xyz + 2.0 * cross(rotationQuat.xyz, tempNor);

  float stretch_factor = (abs(dot(normalize(M * vec4(rotated, 0.0)).xyz, velocity.xyz)));
  stretch_factor /= 6;
  vec3 offsetWorldPos = worldVPos + (stretch_factor * (M * vec4(normalize(rotated), 0.0))).xyz;

  fragNor = (M * vec4(rotatedNor, 0.0)).xyz;
  vTexCoord = vertTex;
  fPosLS = LS * vec4((offsetWorldPos).xyz, 1.0);
  /* Calculate light and view vectors used for lighting */
  viewVectorModel = normalize(viewVector - worldVPos);
	lightDir = normalize(lightPos - worldVPos);
  gl_Position = P * V * vec4(offsetWorldPos, 1.0);
}
