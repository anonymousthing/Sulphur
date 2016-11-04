#pragma once

#include "Vector.h"

template<typename ElemType>
struct _VertexPositionTextured {
	_Vector3<ElemType> position;
	_Vector2<ElemType> textureCoordinate;

	inline ElemType getX() { return position.x; }
	inline ElemType getY() { return position.y; }
	inline ElemType getZ() { return position.z; }
	inline ElemType getU() { return textureCoordinate.x; }
	inline ElemType getV() { return textureCoordinate.y; }
};
typedef _VertexPositionTextured<float> VertexPositionTextured;

template<typename ElemType>
struct _VertexPositionNormalTextured {
	_Vector3<ElemType> position;
	_Vector3<ElemType> normal;
	_Vector2<ElemType> textureCoordinate;

	inline ElemType getX() { return position.x; }
	inline ElemType getY() { return position.y; }
	inline ElemType getZ() { return position.z; }
	inline ElemType getNX() { return normal.x; }
	inline ElemType getNY() { return normal.y; }
	inline ElemType getNZ() { return normal.z; }
	inline ElemType getU() { return textureCoordinate.x; }
	inline ElemType getV() { return textureCoordinate.y; }
};
typedef _VertexPositionNormalTextured<float> VertexPositionNormalTextured;
