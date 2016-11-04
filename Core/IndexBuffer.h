#pragma once

#include "GraphicsHandles.h"

#include <stdint.h>
#include <type_traits>

//TODO: Investigate ways we can avoid having this?
//This is purely an impl detail, but templates...
#include <glew\glew.h>
#include "GLTemplateTypeHelper.h"

class VertexArray;

void __indexbuffer_initHelper(VertexArray *varray, size_t glType, size_t stride, size_t count, void *data);

template<typename T>
void CreateIndexBuffer(VertexArray *varray, T *data, size_t indexCount) {
	static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value &&
		sizeof(T) >= 1 && sizeof(T) <= 4, "T must be an unsigned int type, between 1 and 4 bytes long.");

	__indexbuffer_initHelper(varray, OpenGLHelper_ToGLType<T>::Value, sizeof(T), indexCount, data);
}
