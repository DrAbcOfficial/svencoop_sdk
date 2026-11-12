#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "scripting/core/CASVote.h"
#include "scripting/core/CASFunction.h"
#include "scripting/core/CASServerManager.h"
#include "server/CServer.h"
#include "gamerules/CVoteManager.h"
#include "../../../external/angelscript/sdk/add_on/scriptany/scriptany.h"

CASVote::CASVote( const CString &name, const CString &text, float length, float percentage )
    : m_szName( name ), m_szVoteText( text ), m_pVoteBlockedCallback( NULL ),
      m_pVoteEndCallback( NULL ), m_pUserData( NULL )
{
	m_flVoteLength = length < 0.0f ? 30.0f : length;
	m_flPercentageNeeded = percentage < 0.0f || percentage > 100.0f ? 50.0f : percentage;
}

CASVote::~CASVote()
{
	ClearUserData();
	ClearCallbacks();
}

void CASVote::AddRef() const
{
	CASGCBaseClass<CASRefCountedBaseClass>::AddRef();
}

void CASVote::Release() const
{
	m_bGCFlag = false;

	if ( InternalRelease() )
		delete this;
}

void CASVote::EnumReferences( asIScriptEngine *engine )
{
	if ( m_pVoteBlockedCallback )
		engine->GCEnumCallback( m_pVoteBlockedCallback->GetFunction() );

	if ( m_pVoteEndCallback )
		engine->GCEnumCallback( m_pVoteEndCallback->GetFunction() );
}

void CASVote::ReleaseReferences( asIScriptEngine *engine )
{
	ClearCallbacks();
}

void CASVote::ClearCallbacks()
{
	if ( m_pVoteEndCallback )
		m_pVoteEndCallback->Release();

	m_pVoteEndCallback = NULL;

	if ( m_pVoteBlockedCallback )
		m_pVoteBlockedCallback->Release();

	m_pVoteBlockedCallback = NULL;
}

void CASVote::VoteBlocked( float time )
{
	if ( m_pVoteBlockedCallback )
		m_pVoteBlockedCallback->Call( 0, this, time );
}

void CASVote::VoteEnd( bool result, int voters )
{
	if ( m_pVoteEndCallback )
		m_pVoteEndCallback->Call( 0, this, result, voters );
}

const char *CASVote::GetName() const
{
	return m_szName.Data();
}

float CASVote::GetVoteLength() const
{
	return m_flVoteLength;
}

const char *CASVote::GetVoteMessage() const
{
	return m_szVoteText.Data();
}

float CASVote::GetPercentageNeeded() const
{
	return m_flPercentageNeeded;
}

const char *CASVote::GetYesText() const
{
	return m_szYesText.IsEmpty() ? "Yes" : m_szYesText.Data();
}

const char *CASVote::GetNoText() const
{
	// The yes-text length is tested for both button labels
	return m_szYesText.IsEmpty() ? "No" : m_szNoText.Data();
}

const CString &CASVote::GetNameAS() const
{
	return m_szName;
}

const CString &CASVote::GetVoteTextAS() const
{
	return m_szVoteText;
}

const CString &CASVote::GetYesTextAS() const
{
	return m_szYesText;
}

const CString &CASVote::GetNoTextAS() const
{
	return m_szNoText;
}

void CASVote::SetVoteText( const CString &text )
{
	m_szVoteText = text;
}

void CASVote::SetYesText( const CString &text )
{
	m_szYesText = text;
}

void CASVote::SetNoText( const CString &text )
{
	m_szNoText = text;
}

void CASVote::SetVoteBlockedCallback( asIScriptFunction *function )
{
	CASFunction *callback = CASFunction::Create( function, CASServerManager::GetInstance()->m_pActiveModule, true );

	if ( m_pVoteBlockedCallback )
		m_pVoteBlockedCallback->Release();

	m_pVoteBlockedCallback = callback;
}

void CASVote::SetVoteEndCallback( asIScriptFunction *function )
{
	CASFunction *callback = CASFunction::Create( function, CASServerManager::GetInstance()->m_pActiveModule, true );

	if ( m_pVoteEndCallback )
		m_pVoteEndCallback->Release();

	m_pVoteEndCallback = callback;
}

void CASVote::Start()
{
	CServer::GetInstance()->GetVoteManager().StartCustomVote( this );
}

CScriptAny *CASVote::GetUserData()
{
	if ( m_pUserData )
		m_pUserData->AddRef();

	return m_pUserData;
}

void CASVote::SetUserData( CScriptAny *userData )
{
	ClearUserData();
	m_pUserData = userData;
}

void CASVote::ClearUserData()
{
	if ( m_pUserData )
		m_pUserData->Release();

	m_pUserData = NULL;
}

CASVote *CreateVote( const CString &name, const CString &text, float length, float percentage )
{
	return new CASVote( name, text, length, percentage );
}
