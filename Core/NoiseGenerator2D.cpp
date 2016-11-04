#include "NoiseGenerator2D.h"

#include <stdlib.h>

static bool validateDimension(void *input) {
	int value = reinterpret_cast<int>(input);
	if (value < 1 || value > 8192) {
		return false;
	}

	return true;
}

static bool returnTrue(void *input) {
	return true;
}

NoiseGenerator2D::NoiseGenerator2D()
	: Generator() {
	registerParameter("width", (void**)&width, validateDimension);
	registerParameter("height", (void**)&height, validateDimension);
	registerParameter("seed", (void**)&seed, returnTrue);

	height = width = 512;
	seed = -1;
}

NoiseGenerator2D::~NoiseGenerator2D() {

}

float *NoiseGenerator2D::generate() const {
	float *buffer = (float*)malloc(sizeof(float) * width * height);

	_generate(buffer);

	return buffer;
}

bool NoiseGenerator2D::generate(float *buffer, size_t bufferSize) const {
	if (bufferSize < sizeof(float) * width * height) {
		return false;
	}

	_generate(buffer);

	return true;
}
