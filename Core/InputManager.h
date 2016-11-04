#pragma once

#include "KeyboardInput.h"
#include "MouseInput.h"
#include "WindowUser.h"

class InputManager {
	WindowUser *windowUser;
public:
	KeyboardInput *kb;
	MouseInput *ms;

	InputManager(GLFWwindow *window);
	~InputManager() {
		delete kb;
	}

	//FIXME: detach input when window is no longer in focus

	void setOnKeyDown(KeyCallback callback);
	void setOnKeyUp(KeyCallback callback);
	void update();
};
