#pragma once

#include <stdint.h>
#include <vector>

#ifdef WINDOWS
#define snprintf	_snprintf
#endif

//Hide things like Sleep, Timers, etc.
class Platform {
	//A stack of requested resolutions for the OS clock, so that it doesn't
	//need to tick faster than required.
	static std::vector<int> timerResolutionStack;

	static int perfCountFrequency;

public:

	//Sleeps for the given number of milliseconds. Note that if the sleep is shorter than
	//the OS's tick, then the actual time slept may be longer. Use Push/Pop Timer Resolution
	//or uSleep for more accurate sleeps.
	static void Sleep(int milliseconds);

	//Sleeps (or spins) for the given number of microseconds.
	static void uSleep(int microseconds);

	//Save the current OS timer resolution to a stack and then ensure that it is at least hertz.
	static void PushTimerResolution(int hertz);

	//Remove a timer resolution from the timer stack, lowering the system's timer resolution if
	//a higher one is no longer requested.
	static void PopTimerResolution();

	//Retrieves a list of arguments that the running instance was given on start-up.
	static std::vector<const char *> GetCommandLineArguments();

	//Retrieves the full command line that the current instance was launched with.
	static const char *GetCommandLine();

	//Returns the number of milliseconds since an undefined (but constant) epoch.
	static uint64_t GetElapsedMilliseconds();

	//Returns the number of microseconds since an undefined (but constant) epoch.
	static uint64_t GetElapsedMicroseconds();

	//Returns the number of time units since an undefined (but constant) epoch.
	//There are GetElapsedTimeFrequency() time units in a second.
	static uint64_t GetElapsedTime();

	//Retrieves the number of time units (measured by GetElapsedTime()) per second.
	static uint64_t GetElapsedTimeFrequency();

	//Executes the given command
	static int Execute(const char *command);

	static void Init();
};
