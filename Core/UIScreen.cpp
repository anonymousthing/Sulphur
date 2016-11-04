#include "UIScreen.h"
#include "ResLoader.h"

UIScreen::UIScreen(Graphics *graphics) : Screen(graphics) {
}

UIScreen::~UIScreen() {

}

void UIScreen::addSprite(Sprite *sprite) {
    sprites.push_back(sprite);
}

bool UIScreen::update(InputManager *inputManager) {
	return false;
}

void UIScreen::draw(int delta) {
    //Add all our sprites since the sprite buffer is cleared each frame
	for (auto si = sprites.begin(), se = sprites.end(); si != se; ++si) {
		Sprite *sprite = *si;
		graphics->addSprite(sprite);
	}

	graphics->set2d();
	graphics->draw2d();
}
