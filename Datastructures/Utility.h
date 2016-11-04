#pragma once

#include <type_traits>

#define ENABLE_IF(condition)	\
	template<bool en = true, typename = typename std::enable_if<(condition) && en>::type>
#define ENABLE_IF_TEMPLATE(condition, ...) \
	template<__VA_ARGS__, bool en = true, typename = typename std::enable_if<(condition) && en>::type>

