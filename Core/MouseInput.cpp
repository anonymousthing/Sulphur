#include "MouseInput.h"

MouseInput::MouseInput(GLFWwindow *window) : window(window) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	cx = width / 2;
	cy = height / 2;
	glfwSetCursorPos(window, cx, cy);
}

void _scrollCallback() {

}

void MouseInput::poll() {
	WindowUser *windowUser = (WindowUser*)glfwGetWindowUserPointer(window);

	glfwGetCursorPos(window, &x, &y);
	windowUser->mouseInfo->dx = x - cx;
	windowUser->mouseInfo->dy = y - cy;
	windowUser->mouseInfo->x += windowUser->mouseInfo->dx;
	windowUser->mouseInfo->y += windowUser->mouseInfo->dy;
	


	glfwSetCursorPos(window, cx, cy);
}

int MouseInput::getX() {
	WindowUser *windowUser = (WindowUser*)glfwGetWindowUserPointer(window);
	return windowUser->mouseInfo->x;
}

int MouseInput::getY() {
	WindowUser *windowUser = (WindowUser*)glfwGetWindowUserPointer(window);
	return windowUser->mouseInfo->y;
}

int MouseInput::getDeltaX() {
	WindowUser *windowUser = (WindowUser*)glfwGetWindowUserPointer(window);
	return windowUser->mouseInfo->dx;
}

int MouseInput::getDeltaY() {
	WindowUser *windowUser = (WindowUser*)glfwGetWindowUserPointer(window);
	return windowUser->mouseInfo->dy;
}
