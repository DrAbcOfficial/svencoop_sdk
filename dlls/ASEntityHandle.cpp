#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "ASEntityHandle.h"
#include "CASDocumentation.h"
#include "EntityHandle.h"
#include <new>

typedef EntityHandle<CBaseEntity> ScriptEntityHandle;

void ConstructEHandle(void *memory)
{
	new(memory) ScriptEntityHandle;
}

void CopyConstructEHandle(const ScriptEntityHandle &other, void *memory)
{
	new(memory) ScriptEntityHandle(other);
}

void EntityConstructEHandle(CBaseEntity *entity, void *memory)
{
	new(memory) ScriptEntityHandle(entity);
}

void DestructEHandle(ScriptEntityHandle *handle)
{
	handle->~ScriptEntityHandle();
}

bool EHandle_IsValid(ScriptEntityHandle *handle)
{
	return (CBaseEntity *)*handle != NULL;
}

void RegisterASEntityHandle(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Safe way to point to CBaseEntities who may die between frames.",
		"EHandle", sizeof(ScriptEntityHandle), asOBJ_VALUE);
	documentation->RegisterObjectBehaviour("Default constructs this handle.", "EHandle", asBEHAVE_CONSTRUCT,
		"void EHandle()", asFUNCTION(ConstructEHandle), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Copy constructs this handle.", "EHandle", asBEHAVE_CONSTRUCT,
		"void EHandle(const EHandle& in other)", asFUNCTION(CopyConstructEHandle), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Constructs this handle with a reference to the given entity.", "EHandle", asBEHAVE_CONSTRUCT,
		"void EHandle(CBaseEntity@ pEntity)", asFUNCTION(EntityConstructEHandle), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Destructor.", "EHandle", asBEHAVE_DESTRUCT,
		"void DestructEHandle()", asFUNCTION(DestructEHandle), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Returns whether this handle is valid.", "EHandle",
		"bool IsValid() const", asFUNCTION(EHandle_IsValid), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Returns whether this handle is valid.", "EHandle",
		"bool opImplConv() const", asFUNCTION(EHandle_IsValid), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Gets the Entity this handle refers to.\nReturns null if invalid.", "EHandle",
		"CBaseEntity@ GetEntity() const", asMETHODPR(ScriptEntityHandle, operator CBaseEntity *, (), CBaseEntity *), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the Entity this handle refers to.\nReturns null if invalid.", "EHandle",
		"CBaseEntity@ opImplCast() const", asMETHODPR(ScriptEntityHandle, operator CBaseEntity *, (), CBaseEntity *), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Assigns the given entity to this handle.", "EHandle",
		"CBaseEntity@ opAssign(CBaseEntity@ pEntity)", asMETHODPR(ScriptEntityHandle, operator=, (CBaseEntity *), CBaseEntity *), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Makes this handle refer to the same entity as the given handle.", "EHandle",
		"EHandle& opAssign(const EHandle& in other)", asMETHODPR(ScriptEntityHandle, operator=, (const ScriptEntityHandle &), ScriptEntityHandle &), asCALL_THISCALL);
	documentation->RegisterObjectMethod("CBaseEntity serial number. Used to determine if the entity is still valid.", "EHandle",
		"int GetSerialNumber() const", asMETHOD(ScriptEntityHandle, GetSerialNumber), asCALL_THISCALL);
}
