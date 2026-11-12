#include "scripting/api/ASClientSayParameters.h"
#include "scripting/core/CASCommand.h"
#include "scripting/core/CASClassRegistration.h"

CASSayParameters::CASSayParameters(CBasePlayer *player, int sayType, const char *command)
	: m_pPlayer(player), m_iSayType(sayType), m_szCommand(command), m_pArguments(NULL), m_bShouldHide(false)
{
	m_pArguments = new CASCommand(m_szCommand.Data());
}

CASSayParameters::~CASSayParameters()
{
	m_pArguments->Release();
}

void CASSayParameters::Release(void) const
{
	if (InternalRelease())
		delete this;
}

CBasePlayer *CASSayParameters::GetPlayer(void) const
{
	return m_pPlayer;
}

int CASSayParameters::GetSayType(void) const
{
	return m_iSayType;
}

const CString &CASSayParameters::GetCommand(void) const
{
	return m_szCommand;
}

const CASCommand *CASSayParameters::GetArguments(void) const
{
	m_pArguments->AddRef();
	return m_pArguments;
}

bool CASSayParameters::ShouldHide(void) const
{
	return m_bShouldHide;
}

void CASSayParameters::SetShouldHide(bool shouldHide)
{
	m_bShouldHide = shouldHide;
}

void RegisterSCScriptClientSayParameters(CASDocumentation *documentation)
{
	documentation->RegisterEnum("Type of say the client used", "ClientSayType");
	documentation->RegisterEnumValue("Say", "ClientSayType", "CLIENTSAY_SAY", 0);
	documentation->RegisterEnumValue("Say team", "ClientSayType", "CLIENTSAY_SAYTEAM", 1);
	documentation->RegisterObjectType("ClientSay parameters", "SayParameters", 0, asOBJ_REF);
	RegisterSCScriptRefCountedBaseClass<CASSayParameters>(documentation, "SayParameters");
	documentation->RegisterObjectMethod("Gets the player that is saying something", "SayParameters", "CBasePlayer@ GetPlayer() const", asMETHOD(CASSayParameters, GetPlayer), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the say type of this text", "SayParameters", "ClientSayType GetSayType() const", asMETHOD(CASSayParameters, GetSayType), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the entire command string", "SayParameters", "const string& GetCommand() const", asMETHOD(CASSayParameters, GetCommand), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the arguments in this command.", "SayParameters", "const CCommand@ GetArguments() const", asMETHOD(CASSayParameters, GetArguments), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets whether this text will be hidden or not", "SayParameters", "bool get_ShouldHide() const", asMETHOD(CASSayParameters, ShouldHide), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets whether this text will be hidden or not", "SayParameters", "void set_ShouldHide(bool fShouldHide)", asMETHOD(CASSayParameters, SetShouldHide), asCALL_THISCALL);
}
