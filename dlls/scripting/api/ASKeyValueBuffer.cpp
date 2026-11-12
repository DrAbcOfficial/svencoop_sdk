#include "scripting/api/ASKeyValueBuffer.h"
#include "scripting/core/CASDocumentation.h"
#include "utils/CKeyValueBuffer.h"

void RegisterASKeyValueBuffer(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Buffer containing keyvalue pairs", "KeyValueBuffer", 0, asOBJ_REF);
	documentation->RegisterObjectBehaviour("AddRef", "KeyValueBuffer", asBEHAVE_ADDREF,
		"void AddRef()", asMETHODPR(CKeyValueBuffer, AddRef, () const, void), asCALL_THISCALL);
	documentation->RegisterObjectBehaviour("Release", "KeyValueBuffer", asBEHAVE_RELEASE,
		"void Release()", asMETHOD(CKeyValueBuffer, Release), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the client that this buffer belongs to.", "KeyValueBuffer",
		"edict_t@ GetClient() const", asMETHOD(CKeyValueBuffer, GetClient), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets a key value", "KeyValueBuffer",
		"string GetValue(const string& in szKey) const", asMETHOD(CKeyValueBuffer, GetValue), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets a key value.", "KeyValueBuffer",
		"void SetValue(const string& in szKey, const string& in szValue) const",
		asMETHOD(CKeyValueBuffer, SetValue), asCALL_THISCALL);
	documentation->RegisterObjectMethod(
		"Removes a key value. If this is a physics key buffer, this will set the key to an empty string instead.",
		"KeyValueBuffer", "void RemoveValue(const string& in szKey)",
		asMETHOD(CKeyValueBuffer, RemoveValue), asCALL_THISCALL);
}
