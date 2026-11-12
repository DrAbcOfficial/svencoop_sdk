#include "utils/CObjectLifetimeHandler.h"
#include <stddef.h>

CObjectLifetimeHandler g_MapObjectLifetimeHandler;

CObjectLifetimeHandler::CObjectLifetimeHandler()
	: m_pObjects(NULL)
{
}

CObjectLifetimeHandler::~CObjectLifetimeHandler()
{
	Clear();
}

void CObjectLifetimeHandler::AddObject(void *object, ObjectDestructor_t destructor)
{
	if (!object || !destructor)
		return;

	Object_t *entry = new Object_t;

	entry->m_pObject = object;
	entry->m_pfnDestructor = destructor;
	entry->m_pNext = m_pObjects;
	m_pObjects = entry;
}

void CObjectLifetimeHandler::Clear(void)
{
	Object_t *entry = m_pObjects;

	while (entry)
	{
		Object_t *next = entry->m_pNext;

		entry->m_pfnDestructor(entry->m_pObject);
		delete entry;
		entry = next;
	}

	m_pObjects = NULL;
}
