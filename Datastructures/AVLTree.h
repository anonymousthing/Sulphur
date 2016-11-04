#pragma once

#include <stdint.h>
#include <assert.h>
#include "Utility.h"

template<typename TKey, typename TValue, typename Comparator = Less<TKey>>
class AVLTree{
	Comparator compare;

	struct Node{

	};
};