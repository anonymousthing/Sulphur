#include "Camera.h"
#include "Graphics.h"
#include "Matrix.h"

#define PI 3.14159265358979323846

Camera::Camera(Graphics *g) : g(g) {
	view = Matrix4x4::LookAt(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0));
	g->updateProjectionView();
}

void Camera::moveLeft(float amount) {
	position += left * amount;	
}

void Camera::moveRight(float amount) {
	position -= left * amount;
}

void Camera::moveForward(float amount) {
	position += forward * amount;
}

void Camera::moveBack(float amount) {
	position -= forward * amount;
}

void Camera::moveUp(float amount) {
	position.y() += amount;
}

void Camera::moveDown(float amount) {
	position.y() -= amount;
}

void Camera::setPosition(Vector3 pos) {
	position = pos;

	g->updateProjectionView();
}

void Camera::setLookTarget(Vector3 target) {
	dYaw = 0;
	dPitch = 0;
	
	forward = (target - position).normalise();
	updateLeft();

	view = Matrix4x4::LookAt(position, target, up);
	g->updateProjectionView();
}

Vector3 Camera::getPos() {
	return Vector3(view[3][0], view[3][1], view[3][2]);
}

//FIXME: move into the matrix header
Vector3 Camera::multiplyQuatVec3(Vector4 quat, Vector3 vec) {
	Vector3 quatVec = Vector3(quat.data[0], quat.data[1], quat.data[2]);
	Vector3 uv = Vector3::Cross(quatVec, vec);
	Vector3 uuv = Vector3::Cross(quatVec, uv);

	return vec + ((uv * quat.data[3]) + uuv) * 2;
}

//FIXME: move into the matrix header
Vector4 Camera::multipleVec4(Vector4 a, Vector4 b) {
	Vector4 result;

	result[0] = a.data[3] * b.data[0] + a.data[0] * b.data[3] + a.data[1] * b.data[2] - a.data[2] * b.data[1];
	result[1] = a.data[3] * b.data[1] + a.data[1] * b.data[3] + a.data[2] * b.data[0] - a.data[0] * b.data[2];
	result[2] = a.data[3] * b.data[2] + a.data[2] * b.data[3] + a.data[0] * b.data[1] - a.data[1] * b.data[0];
	result[3] = a.data[3] * b.data[3] - a.data[0] * b.data[0] - a.data[1] * b.data[1] - a.data[2] * b.data[2];
	
	return result;
}

//FIXME: move into the matrix header
Vector4 Camera::createQuatAngleAxis(float angle, Vector3 axis) {
	Vector4 result;
	float sAngle = std::sin(angle / 2);
	result[0] = axis[0] * sAngle;
	result[1] = axis[1] * sAngle;
	result[2] = axis[2] * sAngle;
	result[3] = std::cos(angle / 2);

	return result;
}

void Camera::updateLeft() {
	left = -Vector3::Cross(Vector3(forward.x(), 0, forward.z()).normalise(), up);
}

void Camera::update() {
	//Create rotation quaternions based on our current direction axis
	//TODO: Could be done using a rotation matrix, calculate overhead and see which one is faster
	Vector3 axis = Vector3::Cross(forward, up);
	Vector4 pitchQuat = createQuatAngleAxis(dPitch, axis);
	Vector4 headingQuat = createQuatAngleAxis(dYaw, up);
	Vector4 rotationQuat = multipleVec4(pitchQuat, headingQuat).normalise();
	
	//Multiply the rotation quat by our direction vector
	forward = multiplyQuatVec3(rotationQuat, forward);
	//Update left vector while we're at it
	updateLeft();
	
	//Reset dYaw and dPitch
	dYaw = 0;
	dPitch = 0;

	//Update view and projection matrices
	view = Matrix4x4::LookAt(position, position + forward, up);
	g->updateProjectionView();
}

void Camera::rotateBy(float dx, float dy) {
	if (dx)
		dYaw -= dx;

	if (dy)
		dPitch -= dy;
}

