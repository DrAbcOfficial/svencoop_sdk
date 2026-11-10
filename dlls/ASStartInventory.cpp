#include "ASStartInventory.h"
#include "CASDocumentation.h"
#include "CASStartInventory.h"

void RegisterSCScriptStartInventory( CASDocumentation *documentation )
{
	documentation->RegisterObjectType(
	    "Player start inventory manager\n"
	    "Can be used to modify the player's start inventory during a map\n"
	    "Always check if inventory modification is available before trying to make any changes",
	    "CStartInventory", 0, asOBJ_REF | asOBJ_NOCOUNT );

	documentation->RegisterObjectMethod(
	    "Returns whether inventory modification is available.",
	    "CStartInventory", "bool IsAvailable() const",
	    asMETHOD( CASStartInventory, IsAvailable ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether the given entry is present in the inventory.",
	    "CStartInventory", "bool Exists(const string& in szEntry) const",
	    asMETHOD( CASStartInventory, Exists ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Set an item in the inventory.\nSets the amount if the item is already in the inventory.",
	    "CStartInventory", "void Set(const string& in szItem, uint uiAmount)",
	    asMETHODPR( CASStartInventory, Set, ( const CString &, unsigned long ), void ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Set an entry to the inventory.",
	    "CStartInventory", "void Set(const string& in szEntry)",
	    asMETHODPR( CASStartInventory, Set, ( const CString & ), void ), asCALL_THISCALL );

	// These void forwarding methods are declared as returning bool
	documentation->RegisterObjectMethod(
	    "Add or subtract from an existing value.\nInserts if the value is not already in the inventory.",
	    "CStartInventory", "bool Add(const string& in szEntry, uint uiAmount, bool fAdd = true)",
	    asMETHOD( CASStartInventory, Add ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Removes an entry from the inventory.",
	    "CStartInventory", "bool Remove(const string& in szEntry)",
	    asMETHOD( CASStartInventory, Remove ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Restocks the inventory with up to uiAmount of the given item.\n"
	    "Does nothing if the current amount already exceeds the given amount.",
	    "CStartInventory", "bool Restock(const string& in szItem, uint uiAmount)",
	    asMETHOD( CASStartInventory, Restock ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Limits the inventory content of the given item to at most uiAmount.\n"
	    "Does nothing if there is less than uiAmount in the inventory.",
	    "CStartInventory", "bool Limit(const string& in szItem, uint uiAmount)",
	    asMETHOD( CASStartInventory, Limit ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the amount of a given entry.\nReturns 0 if the entry doesn't exist.",
	    "CStartInventory", "uint GetAmount(const string& in szEntry) const",
	    asMETHOD( CASStartInventory, GetAmount ), asCALL_THISCALL );

	documentation->RegisterGlobalProperty(
	    "Player start inventory manager instance", "CStartInventory g_StartInventory",
	    CASStartInventory::GetInstance() );
}
