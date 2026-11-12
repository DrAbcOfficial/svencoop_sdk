#include "utils/CMutex.h"
#include <stddef.h>

CMutex::CMutex()
{
#ifdef _WIN32
	InitializeCriticalSection(&m_Mutex);
#else
	pthread_mutex_init(&m_Mutex, NULL);
#endif
}

CMutex::~CMutex()
{
#ifdef _WIN32
	DeleteCriticalSection(&m_Mutex);
#else
	pthread_mutex_destroy(&m_Mutex);
#endif
}

void CMutex::Lock(void)
{
#ifdef _WIN32
	EnterCriticalSection(&m_Mutex);
#else
	pthread_mutex_lock(&m_Mutex);
#endif
}

void CMutex::Unlock(void)
{
#ifdef _WIN32
	LeaveCriticalSection(&m_Mutex);
#else
	pthread_mutex_unlock(&m_Mutex);
#endif
}

bool CMutex::TryLock(void)
{
#ifdef _WIN32
	return TryEnterCriticalSection(&m_Mutex) != FALSE;
#else
	return pthread_mutex_trylock(&m_Mutex) == 0;
#endif
}
