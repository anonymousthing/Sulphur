#include "InputManager.h"
#include "WindowUser.h"

InputManager::InputManager(GLFWwindow *window) {
	windowUser = new WindowUser();
	glfwSetWindowUserPointer(window, windowUser);

	kb = new KeyboardInput(window);
	ms = new MouseInput(window);
}

void InputManager::setOnKeyDown(KeyCallback callback) {
	windowUser->onKeyDown = callback;
}

void InputManager::setOnKeyUp(KeyCallback callback) {
	windowUser->onKeyUp = callback;
}

void InputManager::update() {
	ms->poll();
}

