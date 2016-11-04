#include "KeyboardInput.h"
#include "Log.h"
#include "WindowUser.h"
#include "Keys.h"

static Log logger("KeyboardInput");

KeyboardInput::KeyboardInput(GLFWwindow *window) : window(window) {
	glfwSetKeyCallback(window, inputCallback);
}

void KeyboardInput::inputCallback(GLFWwindow *window, int key, int scanCode, int state, int mods) {
	//state is either GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
	WindowUser *user = ((WindowUser*)glfwGetWindowUserPointer(window));
	user->keyInfo->lastKeyMap[key] = user->keyInfo->currentKeyMap[key];
	if (state == GLFW_RELEASE) {
		user->keyInfo->currentKeyMap[key] = 0;
		user->onKeyUp((Keys)key, user->keyInfo);
	}
	else if (state == GLFW_PRESS) {
		user->keyInfo->currentKeyMap[key] = 1;
		user->onKeyDown((Keys)key, user->keyInfo);
	}
}

bool KeyboardInput::isDown(Keys k) {
	int key = (int)k;
	if (key < 0 || key >= KEY_COUNT)
		return false;
	//Return true if we're currently down
	WindowUser *user = ((WindowUser*)glfwGetWindowUserPointer(window));
	user->keyInfo->lastKeyMap[key] = user->keyInfo->currentKeyMap[key];
	return user->keyInfo->currentKeyMap[key] != 0;
}

bool KeyboardInput::isJustDown(Keys k) {
	int key = (int)k;
	if (key < 0 || key >= KEY_COUNT)
		return false;
	//Return true if we're currently down, but weren't down last time
	WindowUser *user = ((WindowUser*)glfwGetWindowUserPointer(window));

	return user->keyInfo->currentKeyMap[key] != 0 && user->keyInfo->lastKeyMap[key] == 0;
}
