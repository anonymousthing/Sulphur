#pragma once

#include "Vector.h"

class Graphics;

class Camera {
	Graphics *g;

	Vector3 up = Vector3(0, 1, 0);
	Vector3 forward = Vector3(0, 0, 1);
	Vector3 left = Vector3(-1, 0, 0);
	Vector3 position = Vector3(0, 0, 0);
	float dYaw;
	float dPitch;

	void updateLeft();
	Vector4 multipleVec4(Vector4 a, Vector4 b);
	Vector3 multiplyQuatVec3(Vector4 quat, Vector3 vec);
	Vector4 createQuatAngleAxis(float angle, Vector3 axis);

public:
	Matrix4x4 view;
	Camera(Graphics *g);

	Vector3 getPos();


	void update();

	void moveLeft(float amount);
	void moveRight(float amount);
	void moveForward(float amount);
	void moveBack(float amount);
	void moveUp(float amount);
	void moveDown(float amount);

	void setPosition(Vector3 pos);

	void setLookTarget(Vector3 target);
	void rotateBy(float dx, float dy);
};
