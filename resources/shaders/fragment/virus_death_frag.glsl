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

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
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


	vec3 halfV = (lightDir + viewVector) / length(lightDir + viewVector);
	vec3 colorSpec = MatSpec * (pow(max(dot(fragNor, halfV), 0), shine)) * lightColor;
	color = vec4((diffColor + ambientColor + colorSpec), abs(sin(fragNor.y)));
}

