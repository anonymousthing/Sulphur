#ifdef WINDOWS

#include "DynamicLibrary.h"
#include "WindowsHelper.h"

DynamicLibrary *DynamicLibrary::load(const char *path) {
	void *handle = LoadLibraryA(path);
	
	//Nobody look!
	return (DynamicLibrary*)handle;
}

void *DynamicLibrary::getEntryPoint(const char *entryPointName) {
	return GetProcAddress((HMODULE)this, entryPointName);
}


#endif