#pragma once

#include "Screen.h"
#include "InputManager.h"
#include <vector>
#include "Keys.h"

class ScreenManager {
    std::vector<Screen*> screens;
    Graphics *graphics;
	InputManager *inputManager;
public:
    ScreenManager(Graphics *graphics, InputManager *inputManager);
    ~ScreenManager();
	void addScreen(Screen *screen);
    void destroyScreen(Screen *screen);

    void bringToFront(Screen *screen);
    void sendToBack(Screen *screen);

	void onKeyDown(Keys key, const KeyInfo *keyInfo);
	void onKeyUp(Keys key, const KeyInfo *keyInfo);
	void onMouseDown(int key, const MouseInfo *mouseInfo);
	void onMouseUp(int key, const MouseInfo *mouseInfo);

	void update();
    void draw(int delta);
};
