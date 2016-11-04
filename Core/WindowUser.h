#pragma once

#include <string>
#include <functional>
#include "Keys.h"

#define KEY_COUNT 512

struct KeyInfo {
	char currentKeyMap[KEY_COUNT];
	char lastKeyMap[KEY_COUNT];

	KeyInfo() {
		memset(currentKeyMap, 0, KEY_COUNT);
		memset(lastKeyMap, 0, KEY_COUNT);
	}
};

struct MouseInfo {
	int button;
	int x, y;
	int dx, dy;
	int dz;
};

typedef std::function<void(Keys, KeyInfo*)> KeyCallback;
typedef std::function<void(int, MouseInfo*)> MouseCallback;
typedef std::function<void(int, MouseInfo*)> ScrollCallback;

struct WindowUser {
	//Keyboard info
	KeyCallback onKeyDown;
	KeyCallback onKeyUp;
	KeyInfo *keyInfo;

	//Mouse info
	MouseCallback onMouseDown;
	MouseCallback onMouseUp;
	MouseInfo *mouseInfo;

	WindowUser() {
		keyInfo = new KeyInfo();
		mouseInfo = new MouseInfo();
	}

	~WindowUser() {
		delete keyInfo;
		delete mouseInfo;
	}
};
