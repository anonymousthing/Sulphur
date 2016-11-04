#include "IndexBuffer.h"

#include "VertexArray.h"
#include "GLHandles.h"

#include <glew\glew.h>

void __indexbuffer_initHelper(VertexArray *varray, size_t glType, size_t stride, size_t count, void *data) {
	size_t bufferSize = stride * count;

	glBindVertexArray(decodeHandle(varray->vao));

	GLuint buffer;
	glGenBuffers(1, &buffer);

	//Buffer our data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, data, GL_STATIC_DRAW);

	varray->setIndices(encodeHandle<IndexBufferHandle>(buffer), (int)(count / 3));
}
