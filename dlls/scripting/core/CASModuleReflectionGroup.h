#ifndef CASMODULEREFLECTIONGROUP_H
#define CASMODULEREFLECTIONGROUP_H

#include "scripting/core/IASReflectionGroup.h"

class CASModuleReflectionGroup : public IASReflectionGroup
{
public:
	virtual ~CASModuleReflectionGroup();

	virtual CASFunction *FindGlobalFunction(const CString &name, bool searchByDeclaration);
	virtual unsigned int GetGlobalFunctionCount(void) const;
	virtual CASFunction *GetGlobalFunctionByIndex(unsigned int index);
	virtual CASObjectType *FindObjectType(const CString &name, bool searchByDeclaration);
	virtual unsigned int GetObjectTypeCount(void) const;
	virtual CASObjectType *GetObjectTypeByIndex(unsigned int index);
};

#endif
