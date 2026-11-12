#ifndef CASARGUMENTS_H
#define CASARGUMENTS_H

#include "scripting/core/CASBaseClass.h"
#include "scripting/core/CASArgument.h"
#include <stdarg.h>

class CASArguments : public CASRefCountedBaseClass
{
public:
	CASArguments();
	CASArguments(const CASArguments &other);
	CASArguments(asIScriptGeneric *generic, unsigned int firstArgument);
	CASArguments(asIScriptFunction *function, va_list &arguments);
	~CASArguments();

	void Release(void) const;
	unsigned int GetCount(void) const;
	bool HasArguments(void) const;
	void Assign(const CASArguments &other);
	CASArguments &operator=(const CASArguments &other);
	void Clear(void);
	bool SetArguments(asIScriptGeneric *generic, unsigned int firstArgument);
	bool SetArguments(asIScriptFunction *function, va_list &arguments);

	CASArgument  *m_pArguments;
	unsigned int  m_uiCount;
};

#endif