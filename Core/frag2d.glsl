#version 400 core

in vec2 fragmentUV;

out vec3 color;

uniform sampler2D sampler;

void main() {
	vec2 flippedUV = vec2(fragmentUV.x, 1 - fragmentUV.y);
	color = texture(sampler, flippedUV).rgb;
}
