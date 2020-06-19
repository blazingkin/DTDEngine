#version 330 core
uniform sampler2D Texture0;
uniform sampler2D normalMap;
uniform sampler2D shadowDepth;
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
in float dCo;
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
	vec3 normalMapDir = texture(normalMap, vTexCoord).xyz;

	vec3 lightDir = normalize(lightDir);
	vec3 viewVector = normalize(viewVectorModel);
	vec3 fragNor = normalize(fragNor);

	float lightIntesity = max(dot(lightDir, fragNor), 0);
	vec3 diffColor, ambientColor;
	if (textureEnabled == 1) { 
		diffColor = lightIntesity * texColor0.xyz * lightColor;
		ambientColor =  (MatAmb * lightColor * texColor0.xyz);
	} else {
		diffColor = lightIntesity * MatDif;
		ambientColor =  (MatAmb * lightColor);
	}

	float Shade = TestShadow(fPosLS) * 0.75;

	vec3 halfV = (lightDir + viewVector) / length(lightDir + viewVector);
	vec3 colorSpec = MatSpec * (pow(max(dot(fragNor, halfV), 0), shine)) * lightColor;
	color = vec4(ambientColor + ((1 - Shade) * (diffColor + colorSpec)), texColor0.w);
}

