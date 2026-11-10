#ifndef CASMODELFUNCS_H
#define CASMODELFUNCS_H

#include "CASServerScriptGlobal.h"
#include "CString.h"

class CASModelFuncs : public CASServerScriptGlobal<CASModelFuncs>
{
public:
	virtual ~CASModelFuncs();

	int ModelIndex(const CString &modelName);
	int SetBodygroup(int modelIndex, int body, int group, int value);
	int GetBodygroup(int modelIndex, int body, int group);
	int GetBodygroupSubmodelCount(int modelIndex, int group);
	int GetAttachmentCount(int modelIndex);
	void *GetModelPtr(int modelIndex);
};

#endif
