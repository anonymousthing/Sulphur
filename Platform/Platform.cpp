#ifdef WINDOWS
#include "Platform.h"

#include <xmmintrin.h>
#include "WindowsHelper.h"
#include <shellapi.h>
#include <mmsystem.h>
#undef GetCommandLine

struct StaticInit {
	StaticInit(void(*func)()) { func(); }
};

static StaticInit initCounter(Platform::Init);
std::vector<int> Platform::timerResolutionStack;
int Platform::perfCountFrequency;

static Log logger("Platform");

void Platform::Sleep(int milliseconds) {
	//Forward call to WINAPI.
	::Sleep(milliseconds);
}

void Platform::uSleep(int microseconds) {
	uint64_t currentTime = GetElapsedTime();
	uint64_t targetTime = currentTime + (GetElapsedTimeFrequency() * microseconds / 1000000);
	while (GetElapsedTime() < targetTime) {
		_mm_pause();
	}
}

void Platform::PushTimerResolution(int hertz) {
	timeEndPeriod(timerResolutionStack[timerResolutionStack.size() - 1]);
	timerResolutionStack.push_back(hertz);
	timeBeginPeriod(timerResolutionStack[timerResolutionStack.size() - 1]);	
}

void Platform::PopTimerResolution() {
	timeEndPeriod(timerResolutionStack[timerResolutionStack.size() - 1]);
	timerResolutionStack.pop_back();
	timeBeginPeriod(timerResolutionStack[timerResolutionStack.size() - 1]);
}

std::vector<const char *> Platform::GetCommandLineArguments() {
	int argc;
	LPWSTR *argvw = CommandLineToArgvW(GetCommandLineW(), &argc);
	std::vector<const char *> argv;
	
	size_t convertedChars = 0;
	size_t newsize;

	for (int i = 0; i < argc; i++) {
		newsize = (wcslen(argvw[i]) + 1) * 2;
		char *newString = (char *)malloc(newsize);
		wcstombs_s(&convertedChars, newString, newsize, argvw[i], _TRUNCATE);
		argv.push_back(newString);
	}

	LocalFree(argvw);

	return argv;
}

const char *Platform::GetCommandLine() {
	char *cmdline = GetCommandLineA();
	return cmdline;
}

uint64_t Platform::GetElapsedMilliseconds() {
	return (GetElapsedTime() * 1000 / GetElapsedTimeFrequency());
}

uint64_t Platform::GetElapsedMicroseconds() {
	return (GetElapsedTime() * 1000000 / GetElapsedTimeFrequency());
}

uint64_t Platform::GetElapsedTime() {
	uint64_t time;
	QueryPerformanceCounter((LARGE_INTEGER*)(&time));
	return time;
}

uint64_t Platform::GetElapsedTimeFrequency() {
	return perfCountFrequency;
}

void Platform::Init() {
	QueryPerformanceFrequency((LARGE_INTEGER*)(&perfCountFrequency));
}

int Platform::Execute(const char *command) {
	int returnCode = system(command);

	logger.info("Command exited with code %i.", returnCode);

	return returnCode;
}

#endif
