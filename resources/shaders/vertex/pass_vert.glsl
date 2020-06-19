#version  330 core
layout(location = 0) in vec3 vertPos;

out vec2 texCoord;

void main()
{
   vec2 screenPos = vertPos.xy;
   gl_Position = vec4(vertPos.xyz, 1);
	texCoord = (vertPos.xy+vec2(1, 1))/2.0;
}
