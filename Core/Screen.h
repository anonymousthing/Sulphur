#pragma once

#include "Graphics.h"
#include "InputManager.h"
#include "Sprite.h"
#include "Keys.h"

class Screen {
protected:
	Screen(Graphics *graphics);
	Graphics *graphics;
    std::vector<Sprite*> sprites;
public:
	bool isOverlay = false;
    virtual ~Screen();
	virtual bool onKeyDown(Keys key, const KeyInfo *keyInfo) = 0;
	virtual bool onKeyUp(Keys key, const KeyInfo *keyInfo) = 0;
	virtual bool onMouseDown(int button, const MouseInfo *mouseInfo) = 0;
	virtual bool onMouseUp(int button, const MouseInfo *mouseInfo) = 0;

	virtual bool update(InputManager *inputManager) = 0;
	virtual void draw(int delta) = 0;
    virtual void addSprite(Sprite *sprite);
};
