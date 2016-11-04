#pragma once

#include <stdint.h>

struct UTF8Helper {
	static uint32_t extractNextCodePoint(const char *str, size_t *position);
};
