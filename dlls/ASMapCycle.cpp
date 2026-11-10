#include "ASMapCycle.h"
#include "CASDocumentation.h"
#include "CASMapCycle.h"

void RegisterSCScriptMapCycle( CASDocumentation *documentation )
{
	documentation->RegisterObjectType( "Map cycle read only manager", "CMapCycle", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectMethod(
	    "Gets the map cycle as an array of strings.",
	    "CMapCycle",
	    "array<string>@ GetMapCycle() const",
	    asMETHOD( CASMapCycle, GetMapCycle ),
	    asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns the number of maps in the map cycle.",
	    "CMapCycle",
	    "size_t Count() const",
	    asMETHOD( CASMapCycle, Count ),
	    asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the next map in the cycle.",
	    "CMapCycle",
	    "string GetNextMap() const",
	    asMETHODPR( CASMapCycle, GetNextMap, () const, CString ),
	    asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the next map in the cycle after the given one.",
	    "CMapCycle",
	    "string GetNextMap(const string& in szMapName) const",
	    asMETHODPR( CASMapCycle, GetNextMap, ( const CString & ) const, CString ),
	    asCALL_THISCALL );
	documentation->RegisterGlobalProperty( "Map cycle manager instance", "CMapCycle g_MapCycle", CASMapCycle::GetInstance() );
}
