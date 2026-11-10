// Recast.h
//
// Based on Detour/Recast (public domain)
//

#ifndef RECAST_H
#define RECAST_H

class rcAllocHint
{
public:
	enum Type
	{
		RC_ALLOC_TEMP = 0,	// Temporary allocation, freed as soon as possible
		RC_ALLOC_PERM		// Permanent allocation, never freed for the lifetime of the app
	};
};

void* rcAlloc(int size, rcAllocHint::Type hint = rcAllocHint::RC_ALLOC_TEMP);
void* rcAllocDefault(int size, rcAllocHint::Type hint = rcAllocHint::RC_ALLOC_TEMP);

#endif
