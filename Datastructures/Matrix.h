#pragma once

#include <initializer_list>

#include <assert.h>
#include <memory.h>
#include <math.h>

#include "Utility.h" //For ENABLE_IF

template<typename ElemType, size_t width, size_t height>
struct Matrix {
	typedef typename Matrix<ElemType, width, height>	MyType;

private:
	//Helper to transform literals into ElemType
	template<typename T>
	static ElemType ET(T d) { return (ElemType)d; }

public:
	//We assume column major storage & column vectors.
	// 0 4 8 C
	// 1 5 9 D
	// 2 6 A E
	// 3 7 B F
	ElemType data[width * height];

	Matrix() { }

	Matrix(const ElemType *elems) {
		memcpy(data, elems, sizeof(ElemType)* width * height);
	}

	Matrix(std::initializer_list<ElemType> elements) {
		assert(elements.size() == width * height);

		//Copy the elements over
		ElemType *curData = data;
		for (auto ei = elements.begin(), ee = elements.end(); ei != ee; ++ei) {
			*curData++ = *ei;
		}
	}

	Matrix(const MyType &other) {
		memcpy(data, other.data, sizeof(ElemType)* width * height);
	}
	
private:
	template<int i>
	void construct() { }

	template<int i, typename... Rest>
	inline void construct(ElemType first, Rest... rest) {
		data[i] = first;
		construct<i + 1>(rest...);
	}

public:

	ENABLE_IF_TEMPLATE(sizeof...(Rest)+1 == height && width == 1, typename... Rest)
	Matrix(ElemType first, Rest... rest) {
		data[0] = first;
		construct<1>((ElemType)rest...);
	}

	static void add(MyType &a, const MyType &b) {
		for (int i = 0; i < (width * height); i++) {
			a.data[i] += b.data[i];
		}
	}

	static void add(MyType &dest, const MyType &a, const MyType &b) {
		for (int i = 0; i < (width * height); i++) {
			dest.data[i] = a.data[i] + b.data[i];
		}
	}

	static void sub(MyType &a, const MyType &b) {
		for (int i = 0; i < (width * height); i++) {
			a.data[i] -= b.data[i];
		}
	}

	static void sub(MyType &dest, const MyType &a, const MyType &b) {
		for (int i = 0; i < (width * height); i++) {
			dest.data[i] = a.data[i] - b.data[i];
		}
	}

	template<size_t b_x>
	static void Mul(MyType &a, const MyType &b) {
		MyType dest;

		Mul(dest, a, b);

		a = dest;
	}

	template<size_t a_x, size_t a_y, size_t b_x>
	static void Mul(typename Matrix<ElemType, b_x, a_y> &dest, const Matrix<ElemType, a_x, a_y> &a, const Matrix<ElemType, b_x, a_x> &b) {
		// 0 4 8 C
		// 1 5 9 D
		// 2 6 A E
		// 3 7 B F
		//[x,y]

		for (int a_y1 = 0; a_y1 < a_y; a_y1++) {
			for (int b_x1 = 0; b_x1 < b_x; b_x1++) {
				dest.data[b_x1 * a_y + a_y1] = 0;
				for (int a_x1 = 0; a_x1 < a_x; a_x1++) {
					dest.data[b_x1 * a_y + a_y1] += a.data[a_x1 * a_y + a_y1] * b.data[b_x1 * a_x + a_x1];
				}
			}
		}
	}

	static void mul(MyType &a, ElemType x) {
		for (int i = 0; i < (width * height); i++) {
			a.data[i] *= x;
		}
	}

	static void mul(MyType &dest, const MyType &a, ElemType x) {
		for (int i = 0; i < (width * height); i++) {
			dest.data[i] = a.data[i] * x;
		}
	}

	static void div(MyType &a, ElemType x) {
		for (int i = 0; i < (width * height); i++) {
			a.data[i] /= x;
		}
	}

	static void Div(MyType &dest, const MyType &a, ElemType x) {
		for (int i = 0; i < (width * height); i++) {
			dest.data[i] = a.data[i] / x;
		}
	}

	static MyType Negate(const MyType &x) {
		MyType other;
		for (int i = 0; i < width * height; i++) {
			other.data[i] = -x.data[i];
		}
		return other;
	}

	//Calculates the trace of the given Matrix, or the sum of the elements
	//along the main diagonal (Top-Left to Bottom-Right)
	ENABLE_IF(width == height)
	static ElemType Trace(const MyType &m) {
		ElemType t = ET(0);
		for (int i = 0; i < width; i++) {
			t += m.data[i * width + i];
		}
		return t;
	}

	static MyType Invert(const MyType &m) { }

	//Member delegate functions (Only call to static)

	inline void add(const MyType &other) {
		add(*this, other);
	}

	inline void sub(const MyType &other) {
		sub(*this, other);
	}

	inline void mul(const MyType &other) {
		mul(*this, other);
	}

	inline void mul(ElemType x) {
		mul(*this, x);
	}

	inline void div(ElemType x) {
		div(*this, x);
	}

	MyType negate() const {
		return Negate(*this);
	}

	//Operators (All delegate to static functions)
	MyType operator+(const MyType &other) const {
		MyType result;
		add(result, *this, other);
		return result;
	}

	MyType operator-(const MyType &other) const {
		MyType result;
		sub(result, *this, other);
		return result;
	}

	MyType operator*(const ElemType &x) const {
		MyType result;
		mul(result, *this, x);
		return result;
	}

	MyType operator/(const ElemType &x) const {
		MyType result;
		Div(result, *this, x);
		return result;
	}

	MyType operator-() const {
		return negate();
	}

	MyType operator+() const {
		return (*this);
	}

	template<size_t b_x>
	Matrix<ElemType, b_x, height> operator*(const Matrix<ElemType, b_x, width> &b) {
		Matrix<ElemType, b_x, height> dest;

		Mul(dest, *this, b);

		return dest;
	}

	MyType &operator+=(const MyType &other) {
		add(*this, other);
		return *this;
	}

	MyType &operator-=(const MyType &other) {
		sub(*this, other);
		return *this;
	}

	MyType &operator*=(const MyType &other) {
		mul(*this, other);
		return *this;
	}

	MyType &operator*=(ElemType b) {
		mul(*this, b);
		return *this;
	}

	MyType &operator/=(ElemType b) {
		div(*this, b);
		return *this;
	}

	bool operator==(const MyType &other) const {
		return memcmp(this, &other, sizeof(*this)) == 0;
	}

	bool operator!=(const MyType &other) const {
		return memcmp(this, &other, sizeof(*this)) != 0;
	}

	//--------------------------------------------------
	//---------------- Vector Accessors ----------------
	//--------------------------------------------------

	//Accessors, which will be enabled/disabled by the size of height

	ENABLE_IF(height >= 2 && height <= 4 && width == 1)
	ElemType &x() {
		return data[0];
	}

	ENABLE_IF(height >= 2 && height <= 4 && width == 1)
	const ElemType &x() const {
		return data[0];
	}
	
	ENABLE_IF(height >= 2 && height <= 4 && width == 1)
	ElemType &y() {
		return data[1];
	}

	ENABLE_IF(height >= 2 && height <= 4 && width == 1)
	const ElemType &y() const {
		return data[1];
	}

	ENABLE_IF(height >= 3 && height <= 4 && width == 1)
	ElemType &z() {
		return data[2];
	}

	ENABLE_IF(height >= 3 && height <= 4 && width == 1)
	const ElemType &z() const {
		return data[2];
	}

	ENABLE_IF(height >= 4 && height <= 4 && width == 1)
	ElemType &w() {
		return data[3];
	}

	ENABLE_IF(height >= 4 && height <= 4 && width == 1)
	const ElemType &w() const {
		return data[3];
	}

	//Matrix row operations

	ENABLE_IF(width > 1)
	void setRow(size_t row, std::initializer_list<ElemType> elements) {
		int max = elements.size > width ? width : elements.size;

		int i = 0;
		for (auto ei = elements.begin(), ee = elements.end(); ei != ee && i < max; i++, ++ei) {
			data[i * height + row] = *ei;
		}
	}

	ENABLE_IF_TEMPLATE(width > 1 && sizeof...(rest)+1 == width, typename... Rest)
	void setRow(size_t row, ElemType first, Rest... rest) {
		setRowHelper<0>(row, first, rest...);
	}

private:
	template<size_t i, typename... Rest>
	void setRowHelper(size_t row, ElemType value, Rest ...rest) {
		data[i * height + row] = value;

		setRowHelper<i + 1>(row, rest...);
	}

public:

	//--------------------------------------------------
	//------------ Matrix Utility Functions ------------
	//--------------------------------------------------
	
	ENABLE_IF(width == height)
	static MyType Identity(ElemType scale = (ElemType)1) {
		MyType mat;
		memset(&mat, 0, sizeof(mat));

		for (int i = 0; i < width; i++) {
			mat.data[i * width + i] = scale;
		}

		return mat;
	}

	static MyType Zero() {
		MyType mat;
		memset(&mat, 0, sizeof(MyType));
		return mat;
	}
	
	typename Matrix<ElemType, height, width> transpose() const {
		return Transpose(*this);
	}

	static Matrix<ElemType, height, width> Transpose(const Matrix<ElemType, width, height> &m) {
		Matrix<ElemType, height, width> dest;

		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				dest.get(y, x) = m.get(x, y);
			}
		}

		return dest;
	}

	//LookAt uses Vector3
	ENABLE_IF(height == 4 && width == 4)
	static MyType LookAt(
		const Matrix<ElemType, 1, 3> &pos,
		const Matrix<ElemType, 1, 3> &target,
		const Matrix<ElemType, 1, 3> &up) {

		Matrix<ElemType, 1, 3> facing = (target - pos).normalise();
		Matrix<ElemType, 1, 3> s = Matrix<ElemType, 1, 3>::Cross(facing, up).normalise();
		Matrix<ElemType, 1, 3> u = Matrix<ElemType, 1, 3>::Cross(s, facing);
		
		Matrix4x4 result;

		result[0][0] = s.x();
		result[1][0] = s.y();
		result[2][0] = s.z();

		result[0][1] = u.x();
		result[1][1] = u.y();
		result[2][1] = u.z();

		result[0][2] = -facing.x();
		result[1][2] = -facing.y();
		result[2][2] = -facing.z();
		
		result[3][0] = -Matrix<ElemType, 1, 3>::Dot(s, pos);
		result[3][1] = -Matrix<ElemType, 1, 3>::Dot(u, pos);
		result[3][2] = Matrix<ElemType, 1, 3>::Dot(facing, pos);

		result[0][3] = ET(0);
		result[1][3] = ET(0);
		result[2][3] = ET(0);
		result[3][3] = ET(1);

		return result;
	}

	ENABLE_IF(height == 4 && width == 4)
	static MyType Perspective(ElemType fovY, ElemType aspectRatio, ElemType zNear, ElemType zFar) {
		ElemType tanHalfFovy = tan(fovY / 2);

		MyType result = Zero();
		result.data[0] = 1 / (aspectRatio * tanHalfFovy);
		result.data[5] = 1 / (tanHalfFovy);
		result.data[10] = -(zFar + zNear) / (zFar - zNear);
		result.data[11] = -1;
		result.data[14] = -(2 * zFar * zNear) / (zFar - zNear);

		return result;
	}

	//Creates the specified rotation matrix, rotating around each given axis (ex y changes heading).
	ENABLE_IF(width == 4 && height == 4)
	static MyType Rotation(ElemType x, ElemType y, ElemType z) {
		return RotationX(x) * RotationY(y) * RotationZ(z);
	}

	ENABLE_IF(width == 4 && height == 4)
	static MyType RotationX(ElemType x) {
		return MyType({
			ET(1),	ET(0),	ET(0),	ET(0),	//Col0  0 -  3
			ET(0),	cos(x),	sin(x), ET(0),	//Col1  4 -  7
			ET(0),  -sin(x),cos(x), ET(1),	//Col2  8 - 11
			ET(0),	ET(0),	ET(0),	ET(1)	//Col3 12 - 15
		});
	}

	ENABLE_IF(width == 4 && height == 4)
	static MyType RotationY(ElemType y) {
		return MyType({
			cos(y), ET(0),  -sin(y),ET(0),	//Col0  0 -  3
			ET(0),	ET(1),	ET(0),	ET(0),	//Col1  4 -  7
			sin(y), ET(0),	cos(y), ET(0),	//Col2  8 - 11
			ET(0),	ET(0),	ET(0),	ET(1)	//Col3 12 - 15
		});
	}

	ENABLE_IF(width == 4 && height == 4)
	static MyType RotationZ(ElemType z) {
		return MyType({
			cos(z), sin(z), ET(0),	ET(0),	//Col0  0 -  3
			-sin(z),cos(z),	ET(0),	ET(0),	//Col1  4 -  7
			ET(0),	ET(0),	ET(1),	ET(0),	//Col2  8 - 11
			ET(0),	ET(0),	ET(0),	ET(1)	//Col0 12 - 15
		});
	}

	ENABLE_IF(width == 4 && height == 4)
	static MyType Scaling(ElemType x, ElemType y, ElemType z) {
		return MyType({
			x,		ET(0),	ET(0),	ET(0),
			ET(0),	y,		ET(0),	ET(0),
			ET(0),	ET(0),	z,		ET(0),
			ET(0),	ET(0),	ET(0),	ET(1),
		});
	}

	ENABLE_IF(width == 4 && height == 4)
	static MyType Translation(ElemType x, ElemType y, ElemType z) {
		return MyType({
			ET(1), ET(0), ET(0), ET(0),	//Col0  0 -  3
			ET(0), ET(1), ET(0), ET(0),	//Col1  4 -  7
			ET(0), ET(0), ET(1), ET(0),	//Col2  8 - 11
			x, y, z, ET(1),				//Col3 12 - 15
		});
	}

	//--------------------------------------------------
	//------------ Vector Utility Functions ------------
	//--------------------------------------------------

	ENABLE_IF(width == 1)
	MyType normalise() const {
		ElemType len = length();

		return (*this) / len;
	}

	ENABLE_IF(width == 1)
	ElemType length() const {
		//len = sqrt(x^2 + y^2)
		ElemType squareSum = 0;
		for (int i = 0; i < height; i++) {
			squareSum += data[i] * data[i];
		}
		return sqrt(squareSum);
	}

	ENABLE_IF(width == 1)
	ElemType dot(const MyType &b) const {
		ElemType result = 0;
		for (int i = 0; i < height; i++) {
			result += data[i] * b.data[i];
		}
		return result;
	}

	ENABLE_IF(width == 1)
	static ElemType Dot(const MyType &a, const MyType &b) {
		ElemType result = 0;
		for (int i = 0; i < height; i++) {
			result += a.data[i] * b.data[i];
		}
		return result;
	}
	
	//We have cross product only for Vector3 types.
	ENABLE_IF(width == 1 && height == 3)
	MyType cross(const MyType &b) const {
		MyType result = {
			data[1] * b.data[2] - b.data[1] * data[2],
			data[2] * b.data[0] - b.data[2] * data[0],
			data[0] * b.data[1] - b.data[0] * data[1] };

		return result;
	}

	ENABLE_IF(width == 1 && height == 3)
	static MyType Cross(const MyType &a, const MyType &b) {
		MyType result = {
			a.data[1] * b.data[2] - b.data[1] * a.data[2],
			a.data[2] * b.data[0] - b.data[2] * a.data[0],
			a.data[0] * b.data[1] - b.data[0] * a.data[1] };

		return result;
	}

	//--------------------------------------------------
	//--------------- [] Access Operator ---------------
	//--------------------------------------------------

private:
	struct MatrixAccess {
		ElemType &operator[](int y) {
			return matrix.data[x * height + y];
		}

		const ElemType &operator[](int y) const {
			return matrix.data[x * height + y];
		}

		//We force this as const and remove it -- operator[] takes care of consting for us.
		MatrixAccess(const MyType &matrix, int x)
			: matrix(*((MyType*)&matrix)), x(x) { }

	private:
		MyType &matrix;
		int x;
	};

public:

	//Use Matrix[x][y] for matrices where width > 1
	ENABLE_IF(width > 1)
	ElemType &get(int x, int y) {
		return data[x * height + y];
	}

	ENABLE_IF(width > 1)
	const ElemType &get(int x, int y) const {
		return data[x * height + y];
	}

	ENABLE_IF(width > 1)
	MatrixAccess operator[](int x) {
		return MatrixAccess(*this, x);
	}

	ENABLE_IF(width > 1)
	const MatrixAccess operator[](int x) const {
		return MatrixAccess(*this, x);
	}

	//Use Matrix[y] for matrices where width == 1
	ENABLE_IF(width == 1)
	ElemType &get(int y) {
		return data[x * height + y];
	}

	ENABLE_IF(width == 1)
	const ElemType &get(int y) const {
		return data[x * height + y];
	}

	ENABLE_IF(width == 1)
	ElemType &operator[](int y) {
		return data[y];
	}

	ENABLE_IF(width == 1)
	const ElemType &operator[](int y) const {
		return data[y];
	}
};

template<> template<>
void Matrix<float, 4, 4>::Mul<4, 4, 4>(
	Matrix<float, 4, 4> &dest,
	const Matrix<float, 4, 4> &a,
	const Matrix<float, 4, 4> &b);

extern template
void Matrix<float, 4, 4>::Mul<4, 4, 4>(
	Matrix<float, 4, 4> &dest,
	const Matrix<float, 4, 4> &a,
	const Matrix<float, 4, 4> &b);

template<typename ElemType>
Matrix<typename ElemType, 2, 2> __invert_mat_2x2(const Matrix<typename ElemType, 2, 2> &m) {
	//det = ad - bc
	ElemType det = m.data[0] * m.data[3] - m.data[1] * m.data[2];
	det = 1 / det;

	return Matrix<ElemType, 2, 2>({
		m.data[3] * det,	//A = d
		-m.data[1] * det,	//C = -c
		-m.data[2] * det,	//B = -b
		m.data[0] * det		//D = a
	});
}

//TODO: Template this to make it work for any double/float/etc.

template<>
Matrix<float, 2, 2> Matrix<float, 2, 2>::Invert(const Matrix<float, 2, 2> &m) {
	return __invert_mat_2x2(m);
}

template<>
Matrix<float, 3, 3> Matrix<float, 3, 3>::Invert(const Matrix<float, 3, 3> &m) {
	// A B C 0 3 6
	// D E F 1 4 7
	// G H I 2 5 8
	//det = a(ei - fh) - b(di - fg) + c(dh - eg)
	float det =
		(m.data[0] * (m.data[4] * m.data[8] - m.data[7] * m.data[5])) -
		(m.data[3] * (m.data[1] * m.data[8] - m.data[7] * m.data[2])) +
		(m.data[6] * (m.data[1] * m.data[5] - m.data[4] * m.data[2]));

	return Matrix<float, 3, 3>({
		(m.data[4] * m.data[8] - m.data[7] * m.data[5]), //A =  (ei - fh)
		-(m.data[1] * m.data[8] - m.data[7] * m.data[2]),//B = -(di - fg)
		(m.data[1] * m.data[5] - m.data[4] * m.data[2]), //C =  (dh - eg)
		-(m.data[3] * m.data[8] - m.data[6] * m.data[5]),//D = -(bi - ch)
		(m.data[0] * m.data[8] - m.data[6] * m.data[2]), //E =  (ai - cg)
		-(m.data[0] * m.data[5] - m.data[3] * m.data[2]),//F = -(ah - bg)
		(m.data[3] * m.data[7] - m.data[6] * m.data[4]), //G =  (bf - ce)
		-(m.data[0] * m.data[7] - m.data[6] * m.data[1]),//H = -(af - cd)
		(m.data[0] * m.data[4] - m.data[3] * m.data[1]), //I =  (ae - bd)
	}) * (1 / det);
}

static float __det2x2(float a, float b, float c, float d) {
	return a * d - b * c;
}

template<>
Matrix<float, 4, 4> Matrix<float, 4, 4>::Invert(const Matrix<float, 4, 4> &m) {
	// A B C D	0 4 8  12
	// E F G H	1 5 9  13
	// I J K L	2 6 10 14
	// M N O P	3 7 11 15

	float s0 = __det2x2(m.data[0], m.data[4], m.data[1], m.data[5]);
	float s1 = __det2x2(m.data[0], m.data[8], m.data[1], m.data[9]);
	float s2 = __det2x2(m.data[0], m.data[12], m.data[1], m.data[13]);
	float s3 = __det2x2(m.data[4], m.data[8], m.data[5], m.data[9]);
	float s4 = __det2x2(m.data[4], m.data[12], m.data[5], m.data[13]);
	float s5 = __det2x2(m.data[8], m.data[12], m.data[9], m.data[13]);

	float c0 = __det2x2(m.data[2], m.data[6], m.data[3], m.data[7]);
	float c1 = __det2x2(m.data[2], m.data[10], m.data[3], m.data[11]);
	float c2 = __det2x2(m.data[2], m.data[14], m.data[3], m.data[15]);
	float c3 = __det2x2(m.data[6], m.data[10], m.data[7], m.data[11]);
	float c4 = __det2x2(m.data[6], m.data[14], m.data[7], m.data[15]);
	float c5 = __det2x2(m.data[10], m.data[14], m.data[11], m.data[15]);

	float det = s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0;
	if (det != 0) {
		return MyType({
			(+m.data[5] * c5 - m.data[9] * c4 + m.data[13] * c3),
			(-m.data[1] * c5 + m.data[9] * c2 - m.data[13] * c1),
			(+m.data[1] * c4 - m.data[5] * c2 + m.data[13] * c0),
			(-m.data[1] * c3 + m.data[5] * c1 - m.data[9] * c0),
			
			(-m.data[4] * c5 + m.data[8] * c4 - m.data[12] * c3),
			(+m.data[0] * c5 - m.data[8] * c2 + m.data[12] * c1),
			(-m.data[0] * c4 + m.data[4] * c2 - m.data[12] * c0),
			(+m.data[0] * c3 - m.data[4] * c1 + m.data[8] * c0),

			(+m.data[7] * s5 - m.data[11] * s4 + m.data[15] * s3),
			(-m.data[3] * s5 + m.data[11] * s2 - m.data[15] * s1),
			(+m.data[3] * s4 - m.data[7] * s2 + m.data[15] * s0),
			(-m.data[3] * s3 + m.data[7] * s1 - m.data[11] * s0),

			(-m.data[6] * s5 + m.data[10] * s4 - m.data[14] * s3),
			(+m.data[2] * s5 - m.data[10] * s2 + m.data[14] * s1),
			(-m.data[2] * s4 + m.data[6] * s2 - m.data[14] * s0),
			(+m.data[2] * s3 - m.data[6] * s1 + m.data[10] * s0)
		}) * (1 / det);
	} else {
		return Zero();
	}
}

typedef float				Scalar;

typedef Matrix<Scalar, 4, 4> Matrix4x4;
typedef Matrix<Scalar, 3, 3> Matrix3x3;
typedef Matrix<Scalar, 2, 2> Matrix2x2;

typedef Matrix<float, 4, 4> Matrix4x4F;
typedef Matrix<float, 3, 3> Matrix3x3F;
typedef Matrix<float, 2, 2> Matrix2x2F;

typedef Matrix<double, 4, 4> Matrix4x4D;
typedef Matrix<double, 3, 3> Matrix3x3D;
typedef Matrix<double, 2, 2> Matrix2x2D;

