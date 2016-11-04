#pragma once

#include "glfw\glfw3.h"

#include "Keys.h"

class KeyboardInput {
	GLFWwindow *window;
	static void inputCallback(GLFWwindow *window, int a, int b, int key, int state);
public:
	KeyboardInput(GLFWwindow *window);

	bool isDown(Keys key);
	bool isJustDown(Keys key);
};
