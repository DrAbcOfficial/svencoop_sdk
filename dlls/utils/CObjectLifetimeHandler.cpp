#include "utils/CObjectLifetimeHandler.h"

CObjectLifetimeHandler g_MapObjectLifetimeHandler;

CObjectLifetimeHandler::~CObjectLifetimeHandler()
{
	Clear();
}

void CObjectLifetimeHandler::AddObject(void *object, ObjectDestructor_t destructor)
{
	if (!object || !destructor)
		return;

	m_Objects.push_back( { object, destructor } );
}

// Objects are destroyed in reverse registration order (the same LIFO order
// the old intrusive list produced).
void CObjectLifetimeHandler::Clear(void)
{
	for (auto entry = m_Objects.rbegin(); entry != m_Objects.rend(); ++entry)
		entry->m_pfnDestructor(entry->m_pObject);

	m_Objects.clear();
}
