#pragma once

#include <stdint.h>

//In milliseconds.
#define FPS_FREQUENCY	(999)

class FPS {
private:
	//The number of frames since the last Epoch
	int frameCount;

	//The last time we calculated framerate.
	uint64_t lastEpoch;

	//The current displayed frame-rate
	float currentFrameRate;

public:
	FPS();

	float frame();
	float getFPS() const { return currentFrameRate; }
};
