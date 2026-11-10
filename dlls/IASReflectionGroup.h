#ifndef IASREFLECTIONGROUP_H
#define IASREFLECTIONGROUP_H

class CString;
class CASFunction;
class CASObjectType;

class IASReflectionGroup
{
public:
	virtual ~IASReflectionGroup() = 0;

	virtual CASFunction *FindGlobalFunction(const CString &name, bool searchByDeclaration) = 0;
	virtual unsigned int GetGlobalFunctionCount(void) const = 0;
	virtual CASFunction *GetGlobalFunctionByIndex(unsigned int index) = 0;
	virtual CASObjectType *FindObjectType(const CString &name, bool searchByDeclaration) = 0;
	virtual unsigned int GetObjectTypeCount(void) const = 0;
	virtual CASObjectType *GetObjectTypeByIndex(unsigned int index) = 0;
};

inline IASReflectionGroup::~IASReflectionGroup()
{
}

#endif
