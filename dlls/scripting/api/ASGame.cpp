#include "extdll.h"
#include "scripting/api/ASGame.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASGame.h"

void CASGame_AlertMessage( asIScriptGeneric *generic )
{
	CASGame *game = (CASGame *) generic->GetObject();
	bool result = game->AlertMessage( generic );

	generic->SetReturnByte( result );
}

void RegisterSCScriptAlertType( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Alert type for console messages", "ALERT_TYPE" );
	documentation->RegisterEnumValue( "Prints 'NOTICE: <message>'", "ALERT_TYPE", "at_notice", at_notice );
	documentation->RegisterEnumValue(
	    "Outputs text to the console only if developer is larger than 0",
	    "ALERT_TYPE", "at_console", at_console );
	documentation->RegisterEnumValue(
	    "Only shown if developer is 2 or larger", "ALERT_TYPE", "at_aiconsole", at_aiconsole );
	documentation->RegisterEnumValue( "Prints 'Warning: <message>'", "ALERT_TYPE", "at_warning", at_warning );
	documentation->RegisterEnumValue( "Prints 'Error: <message>'", "ALERT_TYPE", "at_error", at_error );
	documentation->RegisterEnumValue(
	    "Server print to console (only in multiplayer games)", "ALERT_TYPE", "at_logged", at_logged );
}

void RegisterSCScriptGame( CASDocumentation *documentation )
{
	RegisterSCScriptAlertType( documentation );
	documentation->RegisterObjectType( "Game functions", "CGame", 0, asOBJ_REF | asOBJ_NOCOUNT );

	CString declaration( "bool AlertMessage(ALERT_TYPE aType, const string& in szFormat" );

	for ( unsigned int arguments = 0; arguments <= 8; arguments++ )
	{
		CString overload = declaration + ')';

		documentation->RegisterObjectMethod(
		    "Outputs a message to the console. Uses SC printf formatting style.",
		    "CGame", overload.Data(), asFUNCTION( CASGame_AlertMessage ), asCALL_GENERIC );
		declaration += ", ?& in";
	}

	documentation->RegisterObjectMethod(
	    "Precaches a model.\nShuts down the game if not found.\nCan only be called from MapInit.",
	    "CGame", "int PrecacheModel(const string& in szFileName)",
	    asMETHODPR( CASGame, PrecacheModel, (const CString &), int ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Precaches a model.\nShuts down the game if not found.\nCan only be called from entity Precache methods.",
	    "CGame", "int PrecacheModel(CBaseEntity@ pEntity, const string& in szFileName)",
	    asMETHODPR( CASGame, PrecacheModel, (CBaseEntity *, const CString &), int ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Precaches an entity.\n"
	    "This effectively instances an entity with the given class name, calls precache on it, and removes the entity.",
	    "CGame", "void PrecacheOther(const string& in szClassName)",
	    asMETHOD( CASGame, PrecacheOther ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Precaches a monster entity.\n"
	    "This effectively instances a monster entity with the given class name, calls precache on it, and removes the entity.",
	    "CGame", "void PrecacheMonster(const string& in szClassName, bool fAlly)",
	    asMETHOD( CASGame, PrecacheMonster ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Precaches a file for download to clients.", "CGame",
	    "void PrecacheGeneric(const string& in szFileName)",
	    asMETHOD( CASGame, PrecacheGeneric ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns the game name.", "CGame", "string GetGameName() const",
	    asMETHOD( CASGame, GetGameName ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns the game version as a string, e.g. \"5.0\".", "CGame", "string GetGameVersionString() const",
	    asMETHOD( CASGame, GetGameVersionString ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns the game version as a number, e.g. 500:\n"
	    "major version, minor version (3.0 becomes 300, 4.0b1 becomes 401, 4.06 becomes 406, 4.5 becomes 450, etc...)",
	    "CGame", "uint32 GetGameVersion() const",
	    asMETHOD( CASGame, GetGameVersion ), asCALL_THISCALL );
	documentation->RegisterGlobalProperty(
	    "Game functions instance", "CGame g_Game", CASGame::GetInstance() );
}
