#pragma once

#include <stdarg.h>

class Log {

	const char *moduleName;

	void out(const char *message);
	void outf(const char *fmt, va_list ap);

public:
	Log(const char *moduleName);

#ifdef DEBUG
	void debug(const char *);
#else
	inline void debug(...) { }
#endif

	void info(const char *fmt, ...);
	void error(const char *fmt, ...);
	void fatal(const char *fmt, ...);
	void stackTrace();
};
