#include "WindowsHelper.h"

#include <stdio.h>

void __logWindowsError(Log &log, const char *msg, DWORD errorCode) {

	char *errBuff;
	DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
	FormatMessageA(flags, nullptr, errorCode, 0, (char*)&errBuff, 0, nullptr);

	//errBuff contains the formatted error message.
	log.error("%s\n%s", msg, errBuff);
	
	LocalFree(errBuff);
}
