#pragma once

#include "UIControl.h"
#include "Sprite.h"

class UIButton;
typedef std::function<void(UIButton*)> onClickCallback;

class UIButton : UIControl {
public:
	UIButton(Sprite background);
	virtual bool onKeyDown(int key, const KeyInfo *keyInfo) override;
	virtual bool onKeyUp(int key, const KeyInfo *keyInfo) override;

	virtual bool onMouseDown(int button, const MouseInfo *mouseInfo) override;
	virtual bool onMouseUp(int button, const MouseInfo *mouseInfo) override;
};
