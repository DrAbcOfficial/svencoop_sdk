#include "scripting/api/ASModelFuncs.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASModelFuncs.h"

void RegisterSCScriptModelFuncs(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Model functions", "CModelFuncs", 0, asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectMethod(
	    "Gets model index.",
	    "CModelFuncs",
	    "int ModelIndex(const string& in szModelName)",
	    asMETHOD(CASModelFuncs, ModelIndex),
	    asCALL_THISCALL);
	documentation->RegisterObjectMethod(
	    "Builds bodygroup configuration.",
	    "CModelFuncs",
	    "int SetBodygroup(int iModelIndex, int iBody, int iGroup, int iValue)",
	    asMETHOD(CASModelFuncs, SetBodygroup),
	    asCALL_THISCALL);
	documentation->RegisterObjectMethod(
	    "Gets bodygroup configuration.",
	    "CModelFuncs",
	    "int GetBodygroup(int iModelIndex, int iBody, int iGroup)",
	    asMETHOD(CASModelFuncs, GetBodygroup),
	    asCALL_THISCALL);
	documentation->RegisterObjectMethod(
	    "Gets the number of submodels.",
	    "CModelFuncs",
	    "int GetBodygroupSubmodelCount(int iModelIndex, int iGroup)",
	    asMETHOD(CASModelFuncs, GetBodygroupSubmodelCount),
	    asCALL_THISCALL);
	documentation->RegisterObjectMethod(
	    "Gets the number of attachments in a model.",
	    "CModelFuncs",
	    "int GetAttachmentCount(int iModelIndex)",
	    asMETHOD(CASModelFuncs, GetAttachmentCount),
	    asCALL_THISCALL);
	documentation->RegisterGlobalProperty(
	    "Model functions instance", "CModelFuncs g_ModelFuncs", CASModelFuncs::GetInstance());
}
