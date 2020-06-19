#version 330 core
uniform sampler2D Texture0;
uniform sampler2D shadowDepth;

out vec4 Outcolor;

in OUT_struct {
   vec3 fPos;
   vec3 fragNor;
   vec2 vTexCoord;
   vec4 fPosLS;
   vec3 vColor;
} in_struct;

/* returns 1 if shadowed */
/* called with the point projected into the light's coordinate space */

float TestShadow(vec4 LSfPos) {
  float shadow = 0.0;
  vec2 texelSize = 1.0/textureSize(shadowDepth, 0);
  float bias = 0.005f;
  int delta = 1;

  vec3 shifted = (LSfPos.xyz + vec3(1, 1, 1)) * 0.5;

	for (int x = -delta; x <= delta; x++){
    for (int y = -delta; y <= delta; y++){
      vec4 Ld = texture(shadowDepth, shifted.xy + vec2(x, y) * texelSize.r);
      if (Ld.r + bias  < shifted.z) {
        shadow += 1.0;
      }
    }
  }
		return shadow / ((delta + 2.0) * (delta + 2.0));
}

void main() {

  float Shade;
  float amb = 0.3;

  vec4 BaseColor = vec4(in_struct.vColor, 1);
  vec4 texColor0 = texture(Texture0, in_struct.vTexCoord);

  Shade = TestShadow(in_struct.fPosLS);

  Outcolor = amb*(texColor0) + (1.0-Shade)*texColor0*BaseColor;
}

