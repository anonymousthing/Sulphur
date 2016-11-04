#pragma once

#include <stdint.h>

//Usage:
// Some_GL_Function_Needs_Type_Enum(OpenGLHelper_ToGLType<T>::Value);

template<typename T> struct OpenGLHelper_ToGLType;
template<> struct OpenGLHelper_ToGLType<uint8_t> { enum { Value = GL_UNSIGNED_BYTE }; };
template<> struct OpenGLHelper_ToGLType<int8_t> { enum { Value = GL_BYTE }; };
template<> struct OpenGLHelper_ToGLType<int16_t> { enum { Value = GL_SHORT }; };
template<> struct OpenGLHelper_ToGLType<uint16_t> { enum { Value = GL_UNSIGNED_SHORT }; };
template<> struct OpenGLHelper_ToGLType<int32_t> { enum { Value = GL_INT }; };
template<> struct OpenGLHelper_ToGLType<uint32_t> { enum { Value = GL_UNSIGNED_INT }; };
template<> struct OpenGLHelper_ToGLType<float> { enum { Value = GL_FLOAT }; };
template<> struct OpenGLHelper_ToGLType<double> { enum { Value = GL_DOUBLE }; };
