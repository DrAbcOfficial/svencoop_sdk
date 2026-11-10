#ifndef CASBASEOBJECTDATA_H
#define CASBASEOBJECTDATA_H

#include "CASBaseClass.h"
#include <map>
#include <string>

class asITypeInfo;
class CASMethod;
class CASModule;

class CASBaseObjectData : public CASRefCountedBaseClass
{
public:
	CASBaseObjectData(asITypeInfo *type, CASModule *module);
	~CASBaseObjectData();

	unsigned int GetMethodCount(void) const;
	CASMethod *GetMethodByIndex(unsigned int index) const;
	bool MethodExists(const char *declaration);
	CASMethod *GetMethod(const char *declaration);
	void ClearCache(void);

protected:
	friend class CCustomEntityHandler;
	friend class CASEntityFuncs;

	CASModule							*m_pModule;
	asITypeInfo							*m_pTypeInfo;
	std::map<std::string, CASMethod *>	m_MethodCache;
};

#endif
