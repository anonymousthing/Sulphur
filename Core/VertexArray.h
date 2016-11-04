#pragma once

#include "GraphicsHandles.h"
#include "glew\glew.h"
#include <stdlib.h>

class VertexArray {
public:
	VertexArrayHandle vao;
	BufferHandleArray buffers;
	IndexBufferHandle ibuff;
	int vertexCount;
	int primitiveCount;

	VertexArray(VertexArrayHandle vao, BufferHandleArray buffers, int vertexCount)
		: vao(vao), buffers(buffers), vertexCount(vertexCount), ibuff(nullptr) {
	}

	VertexArray(const VertexArray &other) = delete;
	VertexArray(VertexArray &&other)
		: vao(other.vao), buffers(other.buffers), vertexCount(other.vertexCount), ibuff(other.ibuff) {
		other.vao = nullptr;
		other.ibuff = nullptr;
		other.buffers.handles = nullptr;
		other.buffers.count = 0;
		other.vertexCount = other.primitiveCount = 0;
	}

	~VertexArray();

	void setIndices(IndexBufferHandle buff, int primCount) {
		if (ibuff != nullptr) {
			releaseHandle(ibuff);
		}
		ibuff = buff;
		primitiveCount = primCount;
	}
};
