#ifndef CCUSTOMKEYVALUE_H
#define CCUSTOMKEYVALUE_H

#include "CString.h"
#include "server/entvars_types.h"

class Vector;

enum { MAX_CUSTOM_KEYVALUE_TEXT = 512 };

class CCustomKeyvalue
{
public:
	CCustomKeyvalue();
	CCustomKeyvalue(const void *value, entvartype_s type);

	bool Exists(void) const;
	entvartype_s GetType(void) const;
	CString GetString(void) const;
	Vector GetVector(void) const;
	float GetFloat(void) const;
	int GetInteger(void) const;

private:
	const void	*m_pValue;
	entvartype_s	m_Type;
};

#endif
