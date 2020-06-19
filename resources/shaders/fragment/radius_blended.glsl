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
in float dCo;


void main() {
  	vec4 texColor0 = texture(Texture0, vTexCoord);
	vec3 normalMapDir = texture(normalMap, vTexCoord).xyz;

	vec3 lightDir = normalize(lightDir);
	vec3 viewVector = normalize(viewVectorModel);
	vec3 fragNor = normalize(fragNor);//normalize(normalize(fragNor) + (mat3(M) * (normalMapDir - vec3(0.5, 0.5, 0.5))));

	float lightIntesity = max(dot(lightDir, fragNor), 0);
	vec3 diffColor, ambientColor;
	if (textureEnabled == 1) { 
		diffColor = lightIntesity * texColor0.xyz * lightColor;
		ambientColor =  (MatAmb * lightColor * texColor0.xyz);
	} else {
		diffColor = lightIntesity * MatDif * lightColor;
		ambientColor =  (MatAmb * lightColor);
	}


	vec3 halfV = (lightDir + viewVector) / length(lightDir + viewVector);
	vec3 colorSpec = MatSpec * (pow(max(dot(fragNor, halfV), 0), shine)) * lightColor;
	float glowyFactor = dot(normalize(viewVector), normalize(fragNor));
	vec3 glowColor = vec3(0.224, 0.6, 1); // 0x39ff14
	vec3 baseColor = (diffColor + ambientColor + colorSpec);
	vec3 resultingColor = (glowyFactor * baseColor) + ((1 - glowyFactor) * glowColor);
	color = vec4(resultingColor, 1.0);
	color.a = 0.7;
}

