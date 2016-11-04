#pragma once

#include "GraphicsHandles.h"

#include <Vector.h>
#include <Matrix.h>

#include <stdint.h>

class ShaderParameter {
	GenericHandle handle;

public:
	ShaderParameter();

	ShaderParameter(GenericHandle handle);
	~ShaderParameter();

	//Copy/move fine as default.

	void set(float val);
	void set(float val1, float val2);
	void set(float val1, float val2, float val3);
	void set(float val1, float val2, float val3, float val4);

	void set(const Vector2 &val);
	void set(const Vector3 &val);
	void set(const Vector4 &val);

	void set(int32_t val);
	void set(int32_t val1, int32_t val2);
	void set(int32_t val1, int32_t val2, int32_t val3);
	void set(int32_t val1, int32_t val2, int32_t val3, int32_t val4);

	void set(const Vector<int32_t, 2> &val);
	void set(const Vector<int32_t, 3> &val);
	void set(const Vector<int32_t, 4> &val);

	void set(uint32_t val);
	void set(uint32_t val1, uint32_t val2);
	void set(uint32_t val1, uint32_t val2, uint32_t val3);
	void set(uint32_t val1, uint32_t val2, uint32_t val3, uint32_t val4);

	void set(const Vector<uint32_t, 2> &val);
	void set(const Vector<uint32_t, 3> &val);
	void set(const Vector<uint32_t, 4> &val);

	//elems is the number of floats per unit, with there being count units.
	//ex: To set an array of 4 Vector3s, set(&data, 3, 4);
	void set(const float *val, int elems, int count);

	void set(const Vector2 *val, int count);
	void set(const Vector3 *val, int count);
	void set(const Vector4 *val, int count);

	//elems is the number of ints per unit, with there being count units.
	//ex: To set an array of 4 Vector3Is, set(&data, 3, 4);
	void set(const int32_t *val, int elems, int count);

	void set(const Vector<int32_t, 2> *val, int count);
	void set(const Vector<int32_t, 3> *val, int count);
	void set(const Vector<int32_t, 4> *val, int count);

	//elems is the number of uints per unit, with there being count units.
	//ex: To set an array of 4 Vector3Us, set(&data, 3, 4);
	void set(const uint32_t *val, int elems, int count);

	void set(const Vector<uint32_t, 2> *val, int count);
	void set(const Vector<uint32_t, 3> *val, int count);
	void set(const Vector<uint32_t, 4> *val, int count);

	void set(const Matrix2x2 &val);
	void set(const Matrix3x3 &val);
	void set(const Matrix4x4 &val);

	void set(const Matrix2x2 *val, int count);
	void set(const Matrix3x3 *val, int count);
	void set(const Matrix4x4 *val, int count);
};

class Shader {
	ShaderHandle handle;

public:
	Shader() : handle(nullptr) { }

	Shader(ShaderHandle handle)
		: handle(handle) {
	}

	Shader(const Shader &other) = delete;
	Shader(Shader &&other)
		: handle(other.handle) {
		other.handle = nullptr;
	}

	Shader &operator=(const Shader &other) = delete;
	Shader &operator=(Shader &&other) {
		handle = other.handle;
		other.handle = nullptr;

		return *this;
	}

	~Shader() {
		releaseHandle(handle);
	}

	inline ShaderHandle getHandle() { return handle; }

	ShaderParameter getParameter(const char *name);

	void setActive();
};

