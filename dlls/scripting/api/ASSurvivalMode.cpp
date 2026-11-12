#include "extdll.h"
#include "scripting/api/ASSurvivalMode.h"
#include "scripting/core/CASClassRegistration.h"
#include "gamerules/CSurvivalMode.h"

void RegisterSCScriptSurvivalMode( CASDocumentation *documentation )
{
	asDWORD accessMask = documentation->SetDefaultAccessMask( AS_ACCESS_ALL );

	documentation->RegisterObjectType( "Survival Mode handler", "CSurvivalMode", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectMethod(
		"Returns whether Survival Mode is enabled or not.", "CSurvivalMode", "bool IsEnabled() const",
		asMETHOD( CSurvivalMode, IsEnabled ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Enable Survival Mode", "CSurvivalMode", "void Enable( bool fActivateNow = false )",
		asMETHOD( CSurvivalMode, Enable ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Disable Survival Mode", "CSurvivalMode", "void Disable()",
		asMETHOD( CSurvivalMode, Disable ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Toggles Survival Mode.", "CSurvivalMode", "void Toggle()",
		asMETHOD( CSurvivalMode, Toggle ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Returns whether Survival Mode is active or not.", "CSurvivalMode", "bool IsActive() const",
		asMETHOD( CSurvivalMode, IsActive ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Activate Survival Mode", "CSurvivalMode", "void Activate( bool fActivateNow = false )",
		asMETHOD( CSurvivalMode, Activate ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Returns Survival Mode vote state.", "CSurvivalMode", "int GetVoteState()",
		asMETHOD( CSurvivalMode, GetVoteState ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Toggles Survival Mode vote state.", "CSurvivalMode", "void VoteToggle()",
		asMETHOD( CSurvivalMode, VoteToggle ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Resets the Survival Mode vote state to undefined.", "CSurvivalMode", "void ResetVoteState()",
		asMETHOD( CSurvivalMode, ResetVoteState ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Tells the game that the current map wants to use Survival Mode. Should be called in MapInit. Cannot be disabled once set.",
		"CSurvivalMode", "void EnableMapSupport()",
		asMETHOD( CSurvivalMode, EnableMapSupport ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Returns whether the current map supports Survival Mode or not.", "CSurvivalMode", "bool MapSupportEnabled()",
		asMETHOD( CSurvivalMode, MapSupportEnabled ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Sets whether Survival Mode should start when the map is loaded.", "CSurvivalMode", "void SetStartOn( bool value )",
		asMETHOD( CSurvivalMode, SetStartOn ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Returns whether Survival Mode should start when the map is loaded.", "CSurvivalMode", "bool GetStartOn()",
		asMETHOD( CSurvivalMode, GetStartOn ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Returns name of the next survival map.", "CSurvivalMode", "const string& GetNextMap(void) const",
		asMETHOD( CSurvivalMode, GetNextMap ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Sets name of the next survival map.", "CSurvivalMode", "void SetNextMap(const string& in szMapName)",
		asMETHOD( CSurvivalMode, SetNextMap ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Can be used to end a round and force a retry to be used.", "CSurvivalMode", "void EndRound()",
		asMETHOD( CSurvivalMode, EndRound ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Returns delay before survival mode starts.", "CSurvivalMode", "float GetDelayBeforeStart() const",
		asMETHOD( CSurvivalMode, GetDelayBeforeStart ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
		"Sets delay before survival mode starts.", "CSurvivalMode", "void SetDelayBeforeStart( float value )",
		asMETHOD( CSurvivalMode, SetDelayBeforeStart ), asCALL_THISCALL );
	documentation->RegisterGlobalProperty(
		"Survival Mode global instance.", "CSurvivalMode g_SurvivalMode", CSurvivalMode::GetInstance() );

	documentation->SetDefaultAccessMask( accessMask );
}
