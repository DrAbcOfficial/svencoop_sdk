#ifndef SVENCOOP_PORTABLE_TIME_H
#define SVENCOOP_PORTABLE_TIME_H

/**
*	@file
*
*	Portable wrappers around the C time API.
*
*	The two supported platforms disagree in two places:
*	  * Windows uses localtime_s/_localtime64_s, POSIX uses localtime_r;
*	  * time_t is always 64-bit on Windows, but may be 32-bit on 32-bit
*	    Linux (the 2038 problem).
*
*	Keeping both differences in one header avoids the localtime() data race
*	on glibc (it returns a shared static buffer there, while MSVC keeps one
*	buffer per thread).
*/

#include <time.h>

// Seconds since the Unix epoch as a 64-bit value.
inline long long PortableTimeNow(void)
{
#ifdef _WIN32
	return (long long) _time64(NULL);
#else
	return (long long) time(NULL);
#endif
}

// Converts a Unix timestamp to local calendar time.  Returns result on
// success and NULL on failure.
inline tm *PortableLocaltime(long long timestamp, tm *result)
{
	if (!result)
		return NULL;

#ifdef _WIN32
	__time64_t value = (__time64_t) timestamp;
	return _localtime64_s(result, &value) == 0 ? result : NULL;
#else
	time_t value = (time_t) timestamp;
	return localtime_r(&value, result);
#endif
}

// Current local calendar time.  Returns result on success and NULL on
// failure.
inline tm *PortableLocaltimeNow(tm *result)
{
	return PortableLocaltime(PortableTimeNow(), result);
}

#endif // SVENCOOP_PORTABLE_TIME_H
