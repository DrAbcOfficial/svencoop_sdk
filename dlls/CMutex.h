#ifndef CMUTEX_H
#define CMUTEX_H

#ifdef _WIN32
#include "Platform.h"
#else
#include <pthread.h>
#endif

class CMutex
{
public:
	CMutex();
	~CMutex();

	void Lock(void);
	void Unlock(void);
	bool TryLock(void);

private:
#ifdef _WIN32
	CRITICAL_SECTION m_Mutex;
#else
	pthread_mutex_t m_Mutex;
#endif
};

#endif
