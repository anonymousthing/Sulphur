#pragma once

#include "NoiseGenerator2D.h"

class PerlinGenerator : public NoiseGenerator2D {
public:
	PerlinGenerator();
	virtual ~PerlinGenerator();

protected:
	virtual void _generate(float *buffer) const override;

private:
	float xScale, yScale;
};
