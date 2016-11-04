#pragma once

#include "Generator.h"

class NoiseGenerator2D : public Generator {
public:
	//mallocs a new buffer large enough to hold the noise and
	//calls the derrived class's implementation of _generate.
	float *generate() const;

	//Ensures that the given buffer is large enough before calling
	//_generate in the derrived class. Returns false iff bufferSize
	//was too small.
	bool generate(float *buffer, size_t bufferSize) const;

protected:
	//In a derrived class, this actually performs the generation.
	virtual void _generate(float *buffer) const = 0;

	NoiseGenerator2D();
	virtual ~NoiseGenerator2D();

	int height, width;
	int seed;
};
