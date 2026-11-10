#ifndef CCSTRINGFACTORY_H
#define CCSTRINGFACTORY_H

#include <angelscript.h>
#include "CStringHash.h"
#include <unordered_map>

class CCStringFactory : public asIStringFactory
{
public:
	virtual ~CCStringFactory();

	virtual const void *GetStringConstant(const char *data, asUINT length);
	virtual int ReleaseStringConstant(const void *string);
	virtual int GetRawStringData(const void *string, char *data, asUINT *length) const;

private:
	typedef std::unordered_map<CString, int> StringPool_t;

	StringPool_t	m_Strings;
};

#endif
