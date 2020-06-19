#version 330 core
uniform sampler2D Texture0;
uniform sampler2D normalMap;
uniform sampler2D shadowDepth;
uniform sampler2D bumpMap;
uniform vec3 MatDif;
uniform vec3 MatAmb;
uniform vec3 MatSpec;
uniform vec3 lightColor;
uniform mat4 M;
uniform int textureEnabled;
uniform float shine;
in vec3 bmLightDir;
in vec3 matColor;
in vec3 viewVectorModel;
out vec4 color;
in vec2 vTexCoord;
in vec4 fPosLS;

float TestShadow(vec4 LSfPos) {
  float shadow = 0.0;
  vec2 texelSize = 1.0/textureSize(shadowDepth, 0);
  float bias = 0.003f;
  int delta = 2;

  vec3 shifted = (LSfPos.xyz + vec3(1, 1, 1)) * 0.5;

	for (int x = -delta; x <= delta; ++x){
    for (int y = -delta; y <= delta; ++y){
      vec4 Ld = texture(shadowDepth, shifted.xy + vec2(x, y) * texelSize.r);
      if (Ld.r + bias  < shifted.z) {
        shadow += 1.0 / (1.0 + abs((x + y) / 2.0));
      }
    }
  }
	return shadow / ((delta + 2.0) * (delta + 2.0));
}

void main() {
  	vec4 texColor0 = texture(Texture0, vTexCoord);
	vec4 bump = texture(bumpMap, vTexCoord);
	vec3 bump2;

	vec3 normalMapDir = texture(normalMap, vTexCoord).xyz;
	// turn off normal map for roads
	if (texColor0.r + 0.02 > texColor0.g){
		bump2 = vec3(0, 0, 1);
	} else{
		bump2 = normalize((2 * bump.xyz) - vec3(1.0));
	}

	vec3 viewVector = normalize(viewVectorModel);

	vec3 diffColor, ambientColor;
	if (textureEnabled == 1) { 
		diffColor = texColor0.xyz * lightColor;
		ambientColor =  (MatAmb * lightColor * texColor0.xyz);
	} else {
		diffColor = MatDif * lightColor;
		ambientColor =  (MatAmb * lightColor);
	}

	if (texColor0.w < 0.1) {
		discard;
	}

	vec3 halfV = (bmLightDir + viewVector) / length(bmLightDir + viewVector);
	vec3 colorSpec = MatSpec * (pow(max(dot(bump2, halfV), 0), shine)) * lightColor;
	float dCo = abs(dot(bump2, normalize(bmLightDir)));
	float dCo_offset = 0.8;
	dCo =  dCo_offset + (1 - dCo_offset) * dCo;
	float Shade = TestShadow(fPosLS) * 0.65;
	color = vec4(ambientColor + ((1 - Shade) * ((diffColor * dCo) + colorSpec)), texColor0.w);
}

