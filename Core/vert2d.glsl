#version 400 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexUV;

out vec2 fragmentUV;

uniform float aspectRatio = 1.0;
uniform vec2 spritePos;

void main() {
	gl_Position = vec4(vertexPosition.x + spritePos.x, (vertexPosition.y * aspectRatio) + spritePos.y, 0.0, 1.0);

	fragmentUV = vertexUV;
}
