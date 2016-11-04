#pragma once

#include "WindowUser.h"
#include "glfw\glfw3.h"

class MouseInput {
	GLFWwindow *window;
	double x, y;
	double cx, cy;
public:
	MouseInput(GLFWwindow *window);
	void poll();
	int getDeltaX();
	int getDeltaY();
	int getX();
	int getY();
};


