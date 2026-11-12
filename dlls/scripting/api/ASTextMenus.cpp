#include "scripting/api/ASTextMenus.h"
#include "scripting/core/CASTextMenu.h"
#include "scripting/core/CASTextMenus.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASClassRegistration.h"
#include <stddef.h>

CASTextMenu *ConstructTextMenu( asIScriptFunction *inputCallback )
{
	return new CASTextMenu( inputCallback );
}

void RegisterSCScriptTextMenu( CASDocumentation *documentation )
{
	RegisterSCScriptGCRefCountedBaseClass<CASTextMenu>( documentation, "CTextMenu" );
	documentation->RegisterObjectBehaviour(
	    "Constructor. You must pass a player slot callback and handle the input yourself.",
	    "CTextMenu", asBEHAVE_FACTORY, "CTextMenu@ CTextMenu(TextMenuPlayerSlotCallback@ inputCB)",
	    asFUNCTION( ConstructTextMenu ), asCALL_CDECL );
	documentation->RegisterObjectMethod(
	    "Gets this text menu's id. If CTextMenus::INVALID_TEXT_MENU_ID, this is an unregistered menu.",
	    "CTextMenu", "TextMenuId_t get_Id() const", asMETHOD( CASTextMenu, GetId ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Returns whether this menu is registered or not.",
	    "CTextMenu", "bool IsRegistered() const", asMETHOD( CASTextMenu, IsRegistered ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Registers this text menu.", "CTextMenu", "bool Register()",
	    asMETHOD( CASTextMenu, Register ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Unregisters this text menu.", "CTextMenu", "void Unregister()",
	    asMETHOD( CASTextMenu, Unregister ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Opens the menu and keeps it open for the given amount of time.\n"
	    "Time must be a positive value. Maximum 255 seconds. Pass 0 for infinite.\n"
	    "You can optionally pass a list of players that receive this menu.",
	    "CTextMenu", "void Open(const int iDisplayTime, const uint page, array<edict_t@>@ pPlayers = null)",
	    asMETHODPR( CASTextMenu, Open, (int, unsigned int, CScriptArray *), void ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Opens the menu and keeps it open for the given amount of time.\n"
	    "Time must be a positive value. Maximum 255 seconds. Pass 0 for infinite.\n"
	    "You must pass the player that receives this menu.",
	    "CTextMenu", "void Open(const int iDisplayTime, const uint page, CBasePlayer@ pPlayer)",
	    asMETHODPR( CASTextMenu, Open, (int, unsigned int, CBasePlayer *), void ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the number of items in this menu.", "CTextMenu", "size_t GetItemCount() const",
	    asMETHOD( CASTextMenu, GetItemCount ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the number of pages in this menu.", "CTextMenu", "uint GetPageCount() const",
	    asMETHOD( CASTextMenu, GetPageCount ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the item at the given index.", "CTextMenu", "const CTextMenuItem@ GetItem(const size_t uiIndex) const",
	    asMETHOD( CASTextMenu, GetItem ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the title.", "CTextMenu", "const string& GetTitle() const",
	    asMETHOD( CASTextMenu, GetTitle ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the title.", "CTextMenu", "void SetTitle(const string& in szTitle)",
	    asMETHOD( CASTextMenu, SetTitle ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Adds an item to the menu.", "CTextMenu", "void AddItem( const string& in szName, any@ pUserData = null)",
	    asMETHOD( CASTextMenu, AddItem ), asCALL_THISCALL );
}

void RegisterSCScriptTextMenus( CASDocumentation *documentation )
{
	documentation->RegisterTypedef( "Menu id.", "TextMenuId_t", "int32" );
	documentation->RegisterObjectType( "Text menu item. Do not store handles to this object.",
	                                   "CTextMenuItem", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectProperty( "Item name. Used for display.", "CTextMenuItem",
	                                       "const string m_szName", offsetof( CASTextMenu::Item, m_szName ) );
	documentation->RegisterObjectProperty( "User data. For internal use.", "CTextMenuItem",
	                                       "any@ m_pUserData", offsetof( CASTextMenu::Item, m_pUserData ) );
	documentation->RegisterObjectType( "Text menu.", "CTextMenu", 0, asOBJ_REF | asOBJ_GC );
	documentation->RegisterFuncDef( "Player slot callback for text menus.",
	                                "void TextMenuPlayerSlotCallback(CTextMenu@ menu, CBasePlayer@ pPlayer, int iSlot, const CTextMenuItem@ pItem)" );
	documentation->RegisterObjectType( "Text menu manager.", "CTextMenus", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectProperty( "Invalid text menu id.", "CTextMenus",
	                                       "const TextMenuId_t INVALID_TEXT_MENU_ID", offsetof( CASTextMenus, INVALID_TEXT_MENU_ID ) );
	documentation->RegisterGlobalProperty( "Text menu manager global instance", "CTextMenus g_TextMenus", CASTextMenus::GetInstance() );
	RegisterSCScriptTextMenu( documentation );
}
