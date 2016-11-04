#include "Screen.h"

Screen::Screen(Graphics *graphics) : graphics(graphics) {
	
}

Screen::~Screen() {
    for (int i = 0; i < sprites.size(); i++) {
        delete sprites[i];
    }
}

void Screen::addSprite(Sprite *sprite) {
	sprites.push_back(sprite);
}
