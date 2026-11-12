#ifndef COBJECTLIFETIMEHANDLER_H
#define COBJECTLIFETIMEHANDLER_H

#include <vector>

class CObjectLifetimeHandler
{
public:
	typedef void (*ObjectDestructor_t)(void *object);

	CObjectLifetimeHandler() = default;
	~CObjectLifetimeHandler();

	void AddObject(void *object, ObjectDestructor_t destructor);
	void Clear(void);

private:
	struct Object_t
	{
		void				*m_pObject;
		ObjectDestructor_t	m_pfnDestructor;
	};

	std::vector<Object_t>	m_Objects;
};

extern CObjectLifetimeHandler g_MapObjectLifetimeHandler;

#endif
