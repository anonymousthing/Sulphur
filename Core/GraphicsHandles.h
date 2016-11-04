#pragma once

struct __HandleArray {
	int count;
	void *handles;
	__HandleArray(void *handles, int count) 
		: handles(handles), count(count) {
	}
};

//Creates an incomplete type __T, a pointer to said type called T,
//and TArray.
#define MAKE_HANDLE(T)	\
struct __##T; \
typedef __##T *T; \
struct T##Array : public __HandleArray{ \
	/*using __HandleArray::__HandleArray; Doesn't seem to work. */\
	T##Array(void *handles, int count) : __HandleArray(handles, count) { } \
}; \
void releaseHandleArray(T##Array handle); \
void releaseHandle(T handle) //Semicolon left to macro-using code.

MAKE_HANDLE(GenericHandle);
MAKE_HANDLE(BufferHandle);
MAKE_HANDLE(TextureHandle);
MAKE_HANDLE(VertexArrayHandle);
MAKE_HANDLE(IndexBufferHandle);
MAKE_HANDLE(ShaderHandle);

#undef MAKE_HANDLE
