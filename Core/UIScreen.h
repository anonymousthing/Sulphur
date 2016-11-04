#pragma once

#include "Screen.h"

class UIScreen : public Screen {
public:
	UIScreen(Graphics *graphics);
    virtual ~UIScreen();
	virtual bool update(InputManager *inputManager) override;
    virtual void addSprite(Sprite *sprite) override;
	virtual void draw(int delta) override;
};
