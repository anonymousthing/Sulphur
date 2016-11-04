#pragma once
#include "GraphicsHandles.h"

#include <glew\glew.h>

GLuint decodeHandle(GenericHandle handle);
GLuint decodeHandle(BufferHandle handle);
GLuint decodeHandle(TextureHandle handle);
GLuint decodeHandle(VertexArrayHandle handle);
GLuint decodeHandle(IndexBufferHandle handle);
GLuint decodeHandle(ShaderHandle handle);

template<typename T>
GLuint *decodeAddress(T &handle) {
	return (GLuint*)&handle;
}

template<typename T>
T encodeHandle(GLuint handle) {
	return reinterpret_cast<T>(handle);
}

template<typename T>
T encodeHandleArray(GLuint *handles, int count) {
	T handleArray(handles, count);
	return handleArray;
}
