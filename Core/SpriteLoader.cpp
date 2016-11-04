#include "SpriteLoader.h"
#include "ResLoader.h"
#include "GLHandles.h"
#include "Graphics.h"

Sprite *SpriteLoader::loadSprite(const char *path, int x, int y, int width, int height, Graphics *g) {
	float sWidth = width * g->getPixelWidth();
	float sHeight = height * g->getPixelHeight();
	float corners[] = {
		0, 0,
		sWidth, 0,
		0, sHeight,
		sWidth, sHeight
	};

	float uv[] = {
		0, 0,
		1, 0,
		0, 1,
		1, 1
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint *buffers = (GLuint*)malloc(sizeof(GLuint) * 2);
	glGenBuffers(2, buffers);

	int vertexCount = 4;
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount * 2, corners, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount * 2, uv, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	TextureHandle tex = ResLoader::loadTexture(path);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	VertexArrayHandle vertexArray = encodeHandle<VertexArrayHandle>(vao);
	BufferHandleArray bufferArray = encodeHandleArray<BufferHandleArray>(buffers, 2);
	return new Sprite(new VertexArray(vertexArray, bufferArray, vertexCount), new Texture(tex), x, y, g);
}
