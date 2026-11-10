#include "extdll.h"
#include "ASClassicMode.h"
#include "CASClassRegistration.h"
#include "CClassicMode.h"

void RegisterSCScriptClassicMode( CASDocumentation *documentation )
{
	asDWORD accessMask = documentation->SetDefaultAccessMask( AS_ACCESS_MAP );

	documentation->RegisterObjectType( "Item mapping for classic mode.", "ItemMapping", 0, asOBJ_REF );
	RegisterSCScriptRefCountedBaseClass<CClassicMode::CItemMapping>( documentation, "ItemMapping" );
	documentation->RegisterObjectBehaviour(
	    "Constructor", "ItemMapping", asBEHAVE_FACTORY,
	    "ItemMapping@ ItemMapping(const string& in szFrom, const string& in szTo)",
	    asFUNCTION( ConstructItemMapping ), asCALL_CDECL );
	documentation->RegisterObjectMethod(
	    "Gets the From item name", "ItemMapping", "const string& get_From() const",
	    asMETHOD( CClassicMode::CItemMapping, GetFrom ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the To item name", "ItemMapping", "const string& get_To() const",
	    asMETHOD( CClassicMode::CItemMapping, GetTo ), asCALL_THISCALL );

	documentation->RegisterObjectType( "Classic Mode handler", "CClassicMode", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectMethod(
	    "Returns whether the Classic Mode state setting is defined. If not defined, no voting has taken place to choose whether or not to enable Classic Mode.",
	    "CClassicMode", "bool IsStateDefined() const",
	    asMETHOD( CClassicMode, IsStateDefined ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Resets the Classic Mode state setting to undefined.",
	    "CClassicMode", "void ResetState()",
	    asMETHOD( CClassicMode, ResetState ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether Classic Mode is enabled or not.",
	    "CClassicMode", "bool IsEnabled() const",
	    asMETHOD( CClassicMode, IsEnabled ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Enable or disable Classic Mode.",
	    "CClassicMode", "void SetEnabled(const bool fEnabled)",
	    asMETHOD( CClassicMode, SetEnabled ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Forces item remapping even if classic mode is disabled.",
	    "CClassicMode", "void ForceItemRemap(const bool fEnabled)",
	    asMETHOD( CClassicMode, ForceItemRemap ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Toggles Classic Mode.", "CClassicMode", "void Toggle()",
	    asMETHOD( CClassicMode, Toggle ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Tells the game that the current map wants to use Classic Mode. Must be called in MapInit. Cannot be disabled once set."
	    "This will ask players whether they want to enable Classic Mode. Reset on map change.",
	    "CClassicMode", "void EnableMapSupport()",
	    asMETHOD( CClassicMode, EnableMapSupport ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether the map should restart if Classic Mode is changed or not. Defaults to true if not changed by a script. Reset on map change.",
	    "CClassicMode", "bool ShouldRestartOnChange() const",
	    asMETHOD( CClassicMode, ShouldRestartOnChange ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets whether the map should restart if Classic Mode is changed.",
	    "CClassicMode", "void SetShouldRestartOnChange(const bool bShouldRestart)",
	    asMETHOD( CClassicMode, SetShouldRestartOnChange ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the item mappings to use from now on.\nDoes not affect previous mappings.\nPass null to disable all mappings.",
	    "CClassicMode", "void SetItemMappings(array<ItemMapping@>@ pItemMappings)",
	    asMETHOD( CClassicMode, SetItemMappings ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the item mapped to the given item name, or an empty string if no mapping exists.",
	    "CClassicMode", "const string& FindItemMapping(const string& in szItemName) const",
	    asMETHODPR( CClassicMode, FindMappedItem, ( const CString & ) const, const CString & ), asCALL_THISCALL );
	documentation->RegisterGlobalProperty(
	    "Classic Mode global instance.", "CClassicMode g_ClassicMode", CClassicMode::GetInstance() );

	documentation->SetDefaultAccessMask( accessMask );
}
