#ifndef CASOBJECTINSTANCE_H
#define CASOBJECTINSTANCE_H

#include "scripting/core/CASBaseObjectData.h"
#include <stdarg.h>

class CBaseEntity;
struct entvars_s;

class CASObjectInstance : public CASBaseObjectData
{
public:
	CASObjectInstance(asITypeInfo *type, void *object, CASModule *module);
	~CASObjectInstance();

	void Release(void) const;
	void FreeInstance(void);

	bool CallMethod(void *returnValue, bool optional, const char *declaration, int argumentCount, va_list &arguments);
	bool CallMethod(void *returnValue, bool optional, const char *declaration, int argumentCount, ...);
	bool CallMethod(bool optional, const char *declaration, int argumentCount, va_list &arguments);
	bool CallMethod(bool optional, const char *declaration, int argumentCount, ...);
	bool CallMethod(void *returnValue, const char *declaration, int argumentCount, va_list &arguments);
	bool CallMethod(void *returnValue, const char *declaration, int argumentCount, ...);
	bool CallMethod(const char *declaration, int argumentCount, va_list &arguments);
	bool CallMethod(const char *declaration, int argumentCount, ...);

private:
	friend class CCustomEntityHandler;
	friend class CASEntityFuncs;
	friend CBaseEntity *CreateCustomEntity(const char *pszClassName, entvars_s *pev);

	void *m_pObject;
};

#endif
