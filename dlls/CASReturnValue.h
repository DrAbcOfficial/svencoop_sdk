#ifndef CASRETURNVALUE_H
#define CASRETURNVALUE_H

#include "CASBaseClass.h"
#include "CASArgument.h"

class CASBaseCallable;
class CScriptAny;

class CASReturnValue : public CASRefCountedBaseClass
{
public:
	CASReturnValue();
	~CASReturnValue();

	void Release(void) const;
	bool HasReturnValue(void) const;

private:
	friend CScriptAny *CASReturnValue_ToAny(const CASReturnValue *);

	friend class CASBaseCallable;

	CASArgument m_ReturnValue;
	asDWORD     m_uiFlags;
};

#endif
