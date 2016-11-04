#pragma once

#include "Matrix.h"

#include <type_traits>

template<typename ElemType, size_t height>
using Vector = Matrix<ElemType, 1, height>;

typedef Vector<Scalar, 2>	Vector2;
typedef Vector<float, 2>	Vector2F;
typedef Vector<double, 2>	Vector2D;

typedef Vector<Scalar, 3>	Vector3;
typedef Vector<float, 3>	Vector3F;
typedef Vector<double, 3>	Vector3D;

typedef Vector<Scalar, 4>	Vector4;
typedef Vector<float, 4>	Vector4F;
typedef Vector<double, 4>	Vector4D;

template<typename ElemType>
using _Size = Vector<ElemType, 2>;

typedef _Size<Scalar> Size;
typedef _Size<int> SizeI;
typedef _Size<float> SizeF;
typedef _Size<double> SizeD;

template<typename ElemType>
using _Point = Vector<ElemType, 2>;

typedef _Point<Scalar> Point;
typedef _Point<int> PointI;
typedef _Point<float> PointF;
typedef _Point<double> PointD;
