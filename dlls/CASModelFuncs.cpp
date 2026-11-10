#include "extdll.h"
#include "enginecallback.h"
#include "CASModelFuncs.h"
#include "ModelUtils.h"

CASModelFuncs::~CASModelFuncs()
{
}

int CASModelFuncs::ModelIndex(const CString &modelName)
{
	return MODEL_INDEX(modelName.Data());
}

int CASModelFuncs::SetBodygroup(int modelIndex, int body, int group, int value)
{
	return SetBodygroupInt(::GetModelPtr(modelIndex), body, group, value);
}

int CASModelFuncs::GetBodygroup(int modelIndex, int body, int group)
{
	return GetBodygroupInt(::GetModelPtr(modelIndex), body, group);
}

int CASModelFuncs::GetBodygroupSubmodelCount(int modelIndex, int group)
{
	return GetBodygroupSubmodelCountInt(::GetModelPtr(modelIndex), group);
}

int CASModelFuncs::GetAttachmentCount(int modelIndex)
{
	return ::GetAttachmentCount(::GetModelPtr(modelIndex));
}

void *CASModelFuncs::GetModelPtr(int modelIndex)
{
	return ::GetModelPtr(modelIndex);
}
