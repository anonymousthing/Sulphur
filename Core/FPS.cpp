#include "FPS.h"

#include "Platform.h"

FPS::FPS() {
	frameCount = 0;
	lastEpoch = 0;
	currentFrameRate = 0.0f;
}

float FPS::frame() {
	frameCount++;

	uint64_t currentTime = Platform::GetElapsedMilliseconds();
	if (currentTime > lastEpoch + FPS_FREQUENCY) {
		currentFrameRate = (float)frameCount / (((float)FPS_FREQUENCY) / 1000.0f);

		frameCount = 0;
		lastEpoch = currentTime;
	}

	return currentFrameRate;
}

