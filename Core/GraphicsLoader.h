#pragma once

#include "VertexArray.h"
#include "GLHandles.h"

//TODO: Investigate ways we can avoid having this?
//This is purely an impl detail, but templates...
#include <glew\glew.h>
#include "GLTemplateTypeHelper.h"

#include <stdint.h>
#include <malloc.h>

class GraphicsLoader {
public:
	template<typename T, typename... Rest>
	static VertexArray *generateVertices(int vertexCount, T *data, int elemsPerVertex, Rest... rest);
};

//TODO: Move all this to a .tpp?
//Dummy 'finished' function.
template<int i>
static void _generateVertices(GLuint *buffers, int vertexCount) { }

template<int i = 0, typename T, typename... Rest>
static void _generateVertices(GLuint *buffers, int vertexCount, T *data, int elemsPerVertex, Rest... rest) {
	//Buffer our data
	glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertexCount * elemsPerVertex, data, GL_STATIC_DRAW);

	//Ensure that OpenGL knows how to use it.
	glEnableVertexAttribArray(i);
	GLenum type = OpenGLHelper_ToGLType<T>::Value;
	glVertexAttribPointer(i, elemsPerVertex, type, GL_FALSE, 0, (void*)0);

	_generateVertices<i + 1>(buffers, vertexCount, rest...);
}

//TODO: Error check & return nullptr on error.
template<typename T, typename... Rest>
VertexArray *GraphicsLoader::generateVertices(int vertexCount, T *data, int elemsPerVertex, Rest... rest) {
	static_assert(sizeof...(rest) % 2 == 0, "Must have an even number of vertex arguments");
	int numberOfBuffers = sizeof...(rest) / 2 + 1;

	//Create our glVertexArray
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Generate buffers on the GPU
	GLuint *buffers = (GLuint*)malloc(sizeof(GLuint) * numberOfBuffers);
	glGenBuffers(numberOfBuffers, buffers);

	//Buffer our data in
	_generateVertices(buffers, vertexCount, data, elemsPerVertex, rest...);

	VertexArrayHandle vertexArrayHandle = encodeHandle<VertexArrayHandle>(vao);
	BufferHandleArray bufferHandles = encodeHandleArray<BufferHandleArray>(buffers, numberOfBuffers);

	return new VertexArray(vertexArrayHandle, bufferHandles, vertexCount);
}
