#include "Shader.h"

#include "GLHandles.h"

#include <glew\glew.h>

ShaderParameter Shader::getParameter(const char *name) {
	GLint loc = glGetUniformLocation(decodeHandle(handle), name);
	GenericHandle param = encodeHandle<GenericHandle>(loc);

	return ShaderParameter(param);
}

void Shader::setActive() {
	glUseProgram(decodeHandle(handle));
}

//Shader Parameters:

ShaderParameter::ShaderParameter() {
	handle = encodeHandle<GenericHandle>(-1);
}

ShaderParameter::ShaderParameter(GenericHandle handle)
	: handle(handle) {
}

ShaderParameter::~ShaderParameter() {
	//Do nothing, do not release handle.
}

void ShaderParameter::set(float val) {
	glUniform1f(decodeHandle(handle), val);
}

void ShaderParameter::set(float val1, float val2) {
	glUniform2f(decodeHandle(handle), val1, val2);
}

void ShaderParameter::set(float val1, float val2, float val3) {
	glUniform3f(decodeHandle(handle), val1, val2, val3);
}

void ShaderParameter::set(float val1, float val2, float val3, float val4) {
	glUniform4f(decodeHandle(handle), val1, val2, val3, val4);
}

void ShaderParameter::set(const Vector2 &val) {
	glUniform2f(decodeHandle(handle), val.x(), val.y());
}

void ShaderParameter::set(const Vector3 &val) {
	glUniform3f(decodeHandle(handle), val.x(), val.y(), val.z());
}

void ShaderParameter::set(const Vector4 &val) {
	glUniform4f(decodeHandle(handle), val.x(), val.y(), val.z(), val.w());
}

void ShaderParameter::set(int32_t val) {
	glUniform1i(decodeHandle(handle), val);
}

void ShaderParameter::set(int32_t val1, int32_t val2) {
	glUniform2i(decodeHandle(handle), val1, val2);
}

void ShaderParameter::set(int32_t val1, int32_t val2, int32_t val3) {
	glUniform3i(decodeHandle(handle), val1, val2, val3);
}

void ShaderParameter::set(int32_t val1, int32_t val2, int32_t val3, int32_t val4) {
	glUniform4i(decodeHandle(handle), val1, val2, val3, val4);
}

void ShaderParameter::set(const Vector<int32_t, 2> &val) {
	glUniform2i(decodeHandle(handle), val.x(), val.y());
}

void ShaderParameter::set(const Vector<int32_t, 3> &val) {
	glUniform3i(decodeHandle(handle), val.x(), val.y(), val.z());
}

void ShaderParameter::set(const Vector<int32_t, 4> &val) {
	glUniform4i(decodeHandle(handle), val.x(), val.y(), val.z(), val.w());
}

void ShaderParameter::set(uint32_t val) {
	glUniform1ui(decodeHandle(handle), val);
}

void ShaderParameter::set(uint32_t val1, uint32_t val2) {
	glUniform2ui(decodeHandle(handle), val1, val2);
}

void ShaderParameter::set(uint32_t val1, uint32_t val2, uint32_t val3) {
	glUniform3ui(decodeHandle(handle), val1, val2, val3);
}

void ShaderParameter::set(uint32_t val1, uint32_t val2, uint32_t val3, uint32_t val4) {
	glUniform4ui(decodeHandle(handle), val1, val2, val3, val4);
}

void ShaderParameter::set(const Vector<uint32_t, 2> &val) {
	glUniform2ui(decodeHandle(handle), val.x(), val.y());
}

void ShaderParameter::set(const Vector<uint32_t, 3> &val) {
	glUniform3ui(decodeHandle(handle), val.x(), val.y(), val.z());
}

void ShaderParameter::set(const Vector<uint32_t, 4> &val) {
	glUniform4ui(decodeHandle(handle), val.x(), val.y(), val.z(), val.w());
}

void ShaderParameter::set(const float *val, int elems, int count) {
	switch (elems) {
		case 1:
			glUniform1fv(decodeHandle(handle), count, val);
		case 2:
			glUniform2fv(decodeHandle(handle), count, val);
		case 3:
			glUniform3fv(decodeHandle(handle), count, val);
		case 4:
			glUniform4fv(decodeHandle(handle), count, val);
	}
}

void ShaderParameter::set(const Vector2 *val, int count) {
	glUniform2fv(decodeHandle(handle), count, reinterpret_cast<const float*>(val));
}

void ShaderParameter::set(const Vector3 *val, int count) {
	glUniform3fv(decodeHandle(handle), count, reinterpret_cast<const float*>(val));
}

void ShaderParameter::set(const Vector4 *val, int count) {
	glUniform4fv(decodeHandle(handle), count, reinterpret_cast<const float*>(val));
}

void ShaderParameter::set(const int32_t *val, int elems, int count) {
	switch (elems) {
		case 1:
			glUniform1iv(decodeHandle(handle), count, val);
		case 2:
			glUniform2iv(decodeHandle(handle), count, val);
		case 3:
			glUniform3iv(decodeHandle(handle), count, val);
		case 4:
			glUniform4iv(decodeHandle(handle), count, val);
	}
}

void ShaderParameter::set(const Vector<int32_t, 2> *val, int count) {
	glUniform2iv(decodeHandle(handle), count, reinterpret_cast<const int32_t*>(val));
}

void ShaderParameter::set(const Vector<int32_t, 3> *val, int count) {
	glUniform3iv(decodeHandle(handle), count, reinterpret_cast<const int32_t*>(val));
}

void ShaderParameter::set(const Vector<int32_t, 4> *val, int count) {
	glUniform4iv(decodeHandle(handle), count, reinterpret_cast<const int32_t*>(val));
}

void ShaderParameter::set(const uint32_t *val, int elems, int count) {
	switch (elems) {
		case 1:
			glUniform1uiv(decodeHandle(handle), count, val);
		case 2:
			glUniform2uiv(decodeHandle(handle), count, val);
		case 3:
			glUniform3uiv(decodeHandle(handle), count, val);
		case 4:
			glUniform4uiv(decodeHandle(handle), count, val);
	}
}

void ShaderParameter::set(const Vector<uint32_t, 2> *val, int count) {
	glUniform2uiv(decodeHandle(handle), count, reinterpret_cast<const uint32_t*>(val));
}

void ShaderParameter::set(const Vector<uint32_t, 3> *val, int count) {
	glUniform3uiv(decodeHandle(handle), count, reinterpret_cast<const uint32_t*>(val));
}

void ShaderParameter::set(const Vector<uint32_t, 4> *val, int count) {
	glUniform4uiv(decodeHandle(handle), count, reinterpret_cast<const uint32_t*>(val));
}

void ShaderParameter::set(const Matrix2x2 &val) {
	glUniformMatrix2fv(decodeHandle(handle), 1, GL_FALSE, val.data);
}

void ShaderParameter::set(const Matrix3x3 &val) {
	glUniformMatrix3fv(decodeHandle(handle), 1, GL_FALSE, val.data);
}

void ShaderParameter::set(const Matrix4x4 &val) {
	glUniformMatrix4fv(decodeHandle(handle), 1, GL_FALSE, val.data);
}

void ShaderParameter::set(const Matrix2x2 *val, int count) {
	glUniformMatrix2fv(decodeHandle(handle), count, GL_FALSE, reinterpret_cast<const float*>(val));
}

void ShaderParameter::set(const Matrix3x3 *val, int count) {
	glUniformMatrix3fv(decodeHandle(handle), count, GL_FALSE, reinterpret_cast<const float*>(val));
}

void ShaderParameter::set(const Matrix4x4 *val, int count) {
	glUniformMatrix4fv(decodeHandle(handle), count, GL_FALSE, reinterpret_cast<const float*>(val));
}
