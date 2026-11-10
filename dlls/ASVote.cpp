#include "ASVote.h"
#include "CASVote.h"
#include "CASClassRegistration.h"

void RegisterSCScriptVote( CASDocumentation *documentation )
{
	documentation->RegisterObjectType( "Vote class. Can be used to start custom votes.", "Vote", 0, asOBJ_REF | asOBJ_GC );
	documentation->RegisterFuncDef(
	    "Funcdef for vote blocked callbacks. The given time is when the active vote has ended.",
	    "void VoteBlocked(Vote@ pVote, float flTime)" );
	documentation->RegisterFuncDef(
	    "Funcdef for vote ended callbacks. If fResult is true, users voted for yes. Otherwise, voted for no.",
	    "void VoteEnd(Vote@ pVote, bool fResult, int iVoters)" );
	documentation->RegisterObjectBehaviour(
	    "Constructor", "Vote", asBEHAVE_FACTORY,
	    "Vote@ Vote(const string& in szName, const string& in szVoteText, float flVoteLength, float flPercentageNeeded)",
	    asFUNCTION( CreateVote ), asCALL_CDECL );
	RegisterSCScriptGCRefCountedBaseClass<CASVote>( documentation, "Vote" );

	documentation->RegisterObjectMethod(
	    "Gets name of this vote.", "Vote", "const string& GetName() const",
	    asMETHOD( CASVote, GetNameAS ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the vote text.", "Vote", "const string& GetVoteText() const",
	    asMETHOD( CASVote, GetVoteTextAS ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the vote text.", "Vote", "void SetVoteText(const string& in szVoteText)",
	    asMETHOD( CASVote, SetVoteText ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the yes button text.", "Vote", "const string& GetYesText() const",
	    asMETHOD( CASVote, GetYesTextAS ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the yes button text.", "Vote", "void SetYesText(const string& in szYesText)",
	    asMETHOD( CASVote, SetYesText ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the no button text.", "Vote", "const string& GetNoText() const",
	    asMETHOD( CASVote, GetNoTextAS ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the no button text.", "Vote", "void SetNoText(const string& in szNoText)",
	    asMETHOD( CASVote, SetNoText ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the vote blocked callback.", "Vote", "void SetVoteBlockedCallback(VoteBlocked@ pCallback)",
	    asMETHOD( CASVote, SetVoteBlockedCallback ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the vote end callback.", "Vote", "void SetVoteEndCallback(VoteEnd@ pCallback)",
	    asMETHOD( CASVote, SetVoteEndCallback ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Starts this vote.", "Vote", "void Start()",
	    asMETHOD( CASVote, Start ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Gets the user data set on this vote.", "Vote", "any@ GetUserData()",
	    asMETHOD( CASVote, GetUserData ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Sets the user data on this vote.", "Vote", "void SetUserData(any@ pUserData)",
	    asMETHOD( CASVote, SetUserData ), asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Clears the user data set on this vote.", "Vote", "void ClearUserData()",
	    asMETHOD( CASVote, ClearUserData ), asCALL_THISCALL );
}
