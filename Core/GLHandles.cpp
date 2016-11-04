#include "GLHandles.h"

#include <glew\glew.h>

#include <stdlib.h>

GLuint decodeHandle(GenericHandle handle) {
	return reinterpret_cast<GLuint>(handle);
}

GLuint decodeHandle(BufferHandle handle) {
	return reinterpret_cast<GLuint>(handle);
}

GLuint decodeHandle(TextureHandle handle) {
	return reinterpret_cast<GLuint>(handle);
}

GLuint decodeHandle(VertexArrayHandle handle) {
	return reinterpret_cast<GLuint>(handle);
}

GLuint decodeHandle(IndexBufferHandle handle) {
	return reinterpret_cast<GLuint>(handle);
}

GLuint decodeHandle(ShaderHandle handle) {
	return reinterpret_cast<GLuint>(handle);
}

void releaseHandle(BufferHandle handle) {
	GLuint buffer = decodeHandle(handle);
	glDeleteBuffers(1, &buffer);
}

void releaseHandle(IndexBufferHandle handle) {
	GLuint buffer = decodeHandle(handle);
	glDeleteBuffers(1, &buffer);
}

void releaseHandleArray(BufferHandleArray handle) {
	GLuint *buffers = (GLuint*)handle.handles;
	glDeleteBuffers(handle.count, buffers);
	free(buffers);
}

void releaseHandle(TextureHandle handle) {
	GLuint tex = decodeHandle(handle);
	glDeleteTextures(1, &tex);
}

void releaseHandle(VertexArrayHandle handle) {
	//Delete the actual vertex array object
	glDeleteVertexArrays(1, decodeAddress(handle));
}

void releaseHandle(ShaderHandle handle) {
	glDeleteProgram(decodeHandle(handle));
}
