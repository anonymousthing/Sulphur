#pragma once

#include "Sprite.h"

class SpriteLoader {
public:
	static Sprite *loadSprite(const char *path, int x, int y, int width, int height, Graphics *g);
};
