#ifndef CASENGINEREFLECTIONGROUP_H
#define CASENGINEREFLECTIONGROUP_H

#include "scripting/core/IASReflectionGroup.h"

class CASEngineReflectionGroup : public IASReflectionGroup
{
public:
	virtual ~CASEngineReflectionGroup();

	virtual CASFunction *FindGlobalFunction(const CString &name, bool searchByDeclaration);
	virtual unsigned int GetGlobalFunctionCount(void) const;
	virtual CASFunction *GetGlobalFunctionByIndex(unsigned int index);
	virtual CASObjectType *FindObjectType(const CString &name, bool searchByDeclaration);
	virtual unsigned int GetObjectTypeCount(void) const;
	virtual CASObjectType *GetObjectTypeByIndex(unsigned int index);
};

#endif
