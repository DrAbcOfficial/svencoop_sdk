#ifndef CASOBJECTTYPE_H
#define CASOBJECTTYPE_H

#include "scripting/core/CASBaseObjectData.h"

class CASObjectInstance;

class CASObjectType : public CASBaseObjectData
{
public:
	CASObjectType(asITypeInfo *type, CASModule *module);
	~CASObjectType();

	void Release(void) const;
	bool Equals(const CASObjectType *type) const;
	bool IsCompatible(const CASObjectType *type) const;
	bool DerivesFrom(const char *name) const;
	bool DerivesFrom(const CASObjectType *type) const;
	bool HasDefaultConstructor(void) const;
	CASObjectInstance *CreateInstance(void);
};

#endif
