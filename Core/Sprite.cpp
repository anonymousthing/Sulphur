#include "Sprite.h"
#include "Graphics.h"

Sprite::Sprite(VertexArray *vertices, Texture *texture, int x, int y, Graphics *g)
	: vertices(vertices), texture(texture) {
	shouldReleaseTexture = true;
	position.x() = x * g->getPixelWidth();
	position.y() = y * g->getPixelHeight();
}

Sprite::~Sprite() {
	delete vertices;

	if (shouldReleaseTexture) {
		delete texture;
	}
}

void Sprite::move(int dx, int dy, Graphics *g) {
	position.x() += dx * g->getPixelWidth();
	position.y() += dy * g->getPixelHeight();
}

void Sprite::move(float dx, float dy, Graphics *g) {
	position.x() += dx * 2 / 100;
	position.y() += dy * 2 / 100;
}
