#pragma once

#include "Platform.h"
#include "Thread.h"

class AsyncProgress {
private:
	Thread &thread;

public:
	AsyncProgress(Thread &thread) : thread(thread) { }

	//A boolean value representing whether or not the task has completed.
	//This is false if the task has not yet completed. Note that failure
	//counts as completion
	volatile bool completed;

	//A pointer to the result of the task. The value/validity of this pointer is
	//task dependent.
	volatile void *result;

	//A number between 0 and 1000, indicating the % that the task is complete,
	//in increments of 0.1%. If a task does not report progress, this may remain
	//at zero for the duration of the task.
	volatile int progress;

	//Set to false on a running task by default.
	//If this is set to true, then, if the running task supports cancelling, the task
	//will attempt to complete as soon as possible. The effect of cancelling a task is
	//task dependent.
	volatile bool cancel;

	//Polls until the associated task completes, checking every sleepDuration milliseconds.
	inline void await(int sleepDuration = 10) const {
		//During the spin, ensure that the OS timer ticks fast enough for the Sleep.
		Platform::PushTimerResolution((int)(1000.0f / sleepDuration));
		while (!completed) {
			Platform::Sleep(sleepDuration);
		}
		Platform::PopTimerResolution();
	}

	//Force the task to end immediately.
	inline void abort() {
		thread.abort();
	}
};
