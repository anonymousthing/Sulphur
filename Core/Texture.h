#pragma once

#include "GraphicsHandles.h"

class Texture {
	TextureHandle texHandle;
	int width;
	int height;
public:
	TextureHandle getHandle() {
		return texHandle;
	}

	int getWidth() {
		return width;
	}

	int getHeight() {
		return height;
	}

	Texture(TextureHandle texHandle) : texHandle(texHandle), width(width), height(height) { }

	~Texture() {
		releaseHandle(texHandle);
	}

	Texture(Texture &&other) {
		texHandle = other.texHandle;
		other.texHandle = nullptr;
	}

	Texture &operator=(Texture &&other) {
		texHandle = other.texHandle;
		other.texHandle = nullptr;
		return (*this);
	}

	Texture(const Texture &other) = delete;
	Texture &operator=(const Texture &other) = delete;
};


