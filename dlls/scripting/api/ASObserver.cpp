#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player/player.h"
#include "player/observer.h"
#include "scripting/api/ASObserver.h"
#include "scripting/core/CASDocumentation.h"

void RegisterSCScriptObserverClass( CASDocumentation *documentation )
{
	documentation->RegisterObjectType( "Observer mode management class.", "Observer", 0, asOBJ_REF | asOBJ_NOCOUNT );

	documentation->RegisterObjectMethod( "Gets the current observer target, if any.", "Observer",
	                                     "CBaseEntity@ GetObserverTarget()", asMETHOD( CObserver, GetObserverTarget ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Resets the observer target and finds the next suitable candidate.", "Observer",
	                                     "void ResetObserverTarget()", asMETHOD( CObserver, ResetObserverTarget ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Sets the observer target.", "Observer",
	                                     "void SetObserverTarget(CBasePlayer@ pPlayer)", asMETHOD( CObserver, SetObserverTarget ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Returns whether this player can use observer mode.", "Observer",
	                                     "bool CanUseObserver()", asMETHOD( CObserver, CanUseObserver ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Returns whether this player is an observer.", "Observer",
	                                     "bool IsObserver() const", asMETHOD( CObserver, IsObserver ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Starts observer mode.", "Observer",
	                                     "void StartObserver(const Vector& in vecPosition, const Vector& in vecViewAngle, bool fCreateBody = true)", asMETHOD( CObserver, StartObserver ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Stops observer mode.", "Observer",
	                                     "void StopObserver(bool fRespawn)", asMETHOD( CObserver, StopObserver ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Find the next player to observe.", "Observer",
	                                     "void FindNextPlayer(bool fReverse)", asMETHOD( CObserver, FindNextPlayer ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Handles button input.", "Observer",
	                                     "void HandleButtons()", asMETHOD( CObserver, HandleButtons ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Sets the observer mode.", "Observer",
	                                     "void SetMode(ObserverMode iMode)", asMETHOD( CObserver, SetMode ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Find a point to start a player at for observer.", "Observer",
	                                     "CBaseEntity@ FindObserverStartPoint()", asMETHOD( CObserver, FindObserverStartPoint ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Returns whether this player has a corpse.", "Observer",
	                                     "bool HasCorpse()", asMETHOD( CObserver, HasCorpse ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Returns whether this player can control the observer mode.", "Observer",
	                                     "bool CanControlObserverMode() const", asMETHOD( CObserver, CanControlObserverMode ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Sets whether this player can control the observer mode.", "Observer",
	                                     "void SetObserverModeControlEnabled(bool fEnabled)", asMETHOD( CObserver, SetObserverModeControlEnabled ), asCALL_THISCALL );
	documentation->RegisterObjectMethod( "Removes the player's dead body, if it exists.", "Observer",
	                                     "void RemoveDeadBody()", asMETHOD( CObserver, RemoveDeadBody ), asCALL_THISCALL );
}
