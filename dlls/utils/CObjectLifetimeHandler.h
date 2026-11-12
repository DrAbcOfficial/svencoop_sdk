#ifndef COBJECTLIFETIMEHANDLER_H
#define COBJECTLIFETIMEHANDLER_H

class CObjectLifetimeHandler
{
public:
	typedef void (*ObjectDestructor_t)(void *object);

	CObjectLifetimeHandler();
	~CObjectLifetimeHandler();

	void AddObject(void *object, ObjectDestructor_t destructor);
	void Clear(void);

private:
	struct Object_t
	{
		Object_t			*m_pNext;
		void				*m_pObject;
		ObjectDestructor_t	m_pfnDestructor;
	};

	Object_t	*m_pObjects;
};

extern CObjectLifetimeHandler g_MapObjectLifetimeHandler;

#endif
