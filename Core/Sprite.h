#pragma once

#include "GraphicsHandles.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Vector.h"

class Graphics;
class Sprite {
	bool shouldReleaseTexture;

public:
	VertexArray *vertices;
	Texture *texture;
	Vector2 position;

	//int move is per pixel
	void move(int dx, int dy, Graphics *g);
	//float move is a percentage (0-100%)
	void move(float dx, float dy, Graphics *g);

	Sprite(VertexArray *vertices, Texture *texture, int x, int y, Graphics *g);
	~Sprite();

	void setOwnsTexture(bool owns) {
		shouldReleaseTexture = owns;
	}

	//We allow moving
	Sprite(Sprite &&other) {
		shouldReleaseTexture = other.shouldReleaseTexture;
		vertices = other.vertices;
		texture = other.texture;
		other.texture = nullptr;
		other.vertices = nullptr;
	}

	Sprite &operator=(Sprite &&other) {
		shouldReleaseTexture = other.shouldReleaseTexture;
		vertices = other.vertices;
		texture = other.texture;
		other.texture = nullptr;
		other.vertices = nullptr;
		return (*this);
	}

	//But no copying
	Sprite(const Sprite &other) = delete;
	Sprite &operator=(const Sprite &other) = delete;
};
