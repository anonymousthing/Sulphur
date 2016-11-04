#include "VertexArray.h"
#include "GLHandles.h"

VertexArray::~VertexArray() {
	//Delete the vertex buffers/uv buffers/etc
	releaseHandleArray(buffers);
	releaseHandle(vao);
	releaseHandle(ibuff);
}
