#pragma once

#include "Vector.h"

template<typename ElemType>
class _Rectangle {
private:
	typedef typename _Point<ElemType> PointType;
	typedef typename _Size<ElemType> SizeType;

	PointType location;
	SizeType size;

public:
	//Abstracted so we can change location/size to be topleft/bottomright corners later.
	_Rectangle() {}
	_Rectangle(PointType location, SizeType size)
		: location(location), size(size) {
	}

	_Rectangle(ElemType x, ElemType y, ElemType width, ElemType height)
		: location(x, y), size(width, height) {
	}

	inline ElemType getX() { return location.x(); }
	inline ElemType getY() { return location.y(); }
	inline ElemType getWidth() { return size.x(); }
	inline ElemType getHeight() { return size.y(); }

	inline void setX(ElemType value) { location.x = value; }
	inline void setY(ElemType value) { location.y = value; }
	inline void setWidth(ElemType value) { size.x = value; }
	inline void setHeight(ElemType value) { size.y = value; }

	inline PointType getLocation() { return location; }
	inline SizeType getSize() { return size; }

	inline void setLocation(PointType loc) { location = loc; }
	inline void setSize(SizeType sz) { size = sz; }
};

typedef _Rectangle<float> Rectangle;
typedef _Rectangle<int> RectangleI;
typedef _Rectangle<float> RectangleF;
typedef _Rectangle<double> RectangleD;
