#pragma once

#include "GraphicsHandles.h"
#include "VertexArray.h"
#include "Texture.h"

class Model {
public:
	VertexArray *buffers;
	Texture *texture;

	Model(VertexArray *buffers, Texture *texture)
		: buffers(buffers), texture(texture) {
	}

	~Model() {
	//delete buffers;
	//delete texture;
	}

	//We allow moving
	Model(Model &&other) {
		buffers = other.buffers;
		texture = other.texture;
		other.buffers = nullptr;
		other.texture = nullptr;
	}

	Model &operator=(Model &&other) {
		buffers = other.buffers;
		texture = other.texture;
		other.buffers = nullptr;
		other.texture = nullptr;
		return (*this);
	}

	//But no copying
	Model(const Model &other) = delete;
	Model &operator=(const Model &other) = delete;
};
