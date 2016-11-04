#pragma once

#include "WindowUser.h"

class UIControl {
protected:
	//From -1 to 1 where (-1, -1) is bottom left
	float x, y, width, height;
	UIControl();
public:
	virtual bool onKeyDown(int key, const KeyInfo *keyInfo);
	virtual bool onKeyUp(int key, const KeyInfo *keyInfo);

	virtual bool onMouseDown(int button, const MouseInfo *mouseInfo);
	virtual bool onMouseUp(int button, const MouseInfo *mouseInfo);

	virtual	void onUpdate();
};


