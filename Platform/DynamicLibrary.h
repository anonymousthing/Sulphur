#pragma once

class DynamicLibrary {
private:
	void *handle;

public:
	static DynamicLibrary *load(const char *path);

	void *getEntryPoint(const char *entryPointName);
};
