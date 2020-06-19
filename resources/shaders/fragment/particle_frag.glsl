#version 330 core

uniform sampler2D Texture0;
uniform float uTime;

in vec2 vTextureCoords;
in float animationFrame;
out vec4 color;

void main(void){

    float percentLife = animationFrame / 16;
    percentLife = clamp(percentLife, 0.0, 1.0);

    // 16 is the number of atlas sections
    float offset = floor(16.0 * percentLife);
    float offsetX = floor(mod(offset, 4.0))/4.0;
    float offsetY = 0.75 - floor(offset/4.0)/4.0;
    
    vec4 texColor = texture(Texture0, vec2((vTextureCoords.x/4.0)+offsetX, (vTextureCoords.y/4) + offsetY));
    if (texColor.w < 0.75) {
        discard;
    }
    color = texColor;
}
