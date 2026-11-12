#ifndef CASCLASSREGISTRATION_H
#define CASCLASSREGISTRATION_H

#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASLog.h"
#include <string>

template <class T>
void RegisterSCScriptRefCountedBaseClass(CASDocumentation *documentation, const char *name)
{
	documentation->RegisterObjectBehaviour("AddRef", name, asBEHAVE_ADDREF, "void AddRef()", asMETHODPR(T, AddRef, () const, void), asCALL_THISCALL);
	documentation->RegisterObjectBehaviour("Release", name, asBEHAVE_RELEASE, "void Release()", asMETHODPR(T, Release, () const, void), asCALL_THISCALL);
}

template <class T>
void RegisterSCScriptGCRefCountedBaseClass(CASDocumentation *documentation, const char *name)
{
	RegisterSCScriptRefCountedBaseClass<T>(documentation, name);
	documentation->RegisterObjectBehaviour("Get reference count", name, asBEHAVE_GETREFCOUNT, "int GetRefCount() const", asMETHODPR(T, GetRefCount, () const, int), asCALL_THISCALL);
	documentation->RegisterObjectBehaviour("Get GC Flag", name, asBEHAVE_GETGCFLAG, "bool GetGCFlag() const", asMETHODPR(T, GetGCFlag, () const, bool), asCALL_THISCALL);
	documentation->RegisterObjectBehaviour("Set GC Flag", name, asBEHAVE_SETGCFLAG, "void SetGCFlag()", asMETHODPR(T, SetGCFlag, () const, void), asCALL_THISCALL);
	documentation->RegisterObjectBehaviour("Enumerate references held by this class", name, asBEHAVE_ENUMREFS, "void EnumReferences(int& in)", asMETHODPR(T, EnumReferences, (asIScriptEngine *), void), asCALL_THISCALL);
	documentation->RegisterObjectBehaviour("Release all references held by this class", name, asBEHAVE_RELEASEREFS, "void ReleaseReferences(int& in)", asMETHODPR(T, ReleaseReferences, (asIScriptEngine *), void), asCALL_THISCALL);
}

template <class T>
T *ASNotifyGarbageCollector(const char *typeName, T *object)
{
	asITypeInfo *typeInfo = CASServerManager::GetInstance()->GetTypeInfoByName(std::string(typeName));

	if (typeInfo)
		CASServerManager::GetInstance()->m_pEngine->NotifyGarbageCollectorOfNewObject(object, typeInfo);
	else
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "ASNotifyGarbageCollector: failed to retrieve object type '%s' for garbage collection notification!\n", typeName);

	return object;
}

#endif
