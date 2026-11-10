// Recast.cpp
//
// Based on Recast (public domain)
//

#include "Recast.h"

#include <stdlib.h>

static void* (*sRecastAllocFunc)(int size, rcAllocHint::Type hint) = rcAllocDefault;

void* rcAllocDefault(int size, rcAllocHint::Type hint)
{
	(void)hint;
	return malloc(size);
}

void* rcAlloc(int size, rcAllocHint::Type hint)
{
	return sRecastAllocFunc(size, hint);
}
