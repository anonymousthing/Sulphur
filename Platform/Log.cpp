#include "Log.h"

#include <stdio.h>
#include <stdlib.h>

Log::Log(const char *moduleName) : moduleName(moduleName) {
}

void Log::out(const char *message) {
	fputs(message, stderr);
}
void Log::outf(const char *fmt, va_list ap) {
	vfprintf(stderr, fmt, ap);
}

#ifdef DEBUG
void Log::debug(const char *message) {
	out("[");
	out(moduleName);
	out(" - DEBUG] : ");
	out(message);
	out("\n");
}
#endif

void Log::info(const char *fmt, ...) {
	out("[");
	out(moduleName);
	out(" - INFO] : ");

	va_list ap;
	va_start(ap, fmt);
	outf(fmt, ap);
	va_end(ap);

	out("\n");
}

void Log::error(const char *fmt, ...) {
	out("[");
	out(moduleName);
	out(" - ERROR] : ");

	va_list ap;
	va_start(ap, fmt);
	outf(fmt, ap);
	va_end(ap);

	out("\n");
}

void Log::fatal(const char *fmt, ...) {
	out("[");
	out(moduleName);
	out(" - FATAL] : ");

	va_list ap;
	va_start(ap, fmt);
	outf(fmt, ap);
	va_end(ap);

	out("\n");
	abort();
}

#ifdef WINDOWS

#include <Windows.h>
void Log::stackTrace() {
	//TODO: Stack trace
	//CaptureStackBackTrace(0, MAXSHORT, backtrace, nullptr);
}

#else
#error "Platform not supported"
#endif
