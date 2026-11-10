#ifndef CASVOTE_H
#define CASVOTE_H

#include "CBaseVote.h"
#include "CASGCBaseClass.h"
#include "CString.h"

class asIScriptEngine;
class asIScriptFunction;
class CASFunction;
class CScriptAny;

class CASVote : public CBaseVote, public CASGCBaseClass<CASRefCountedBaseClass>
{
public:
	int GetRefCount( void ) const { return CASGCBaseClass<CASRefCountedBaseClass>::GetRefCount(); }
	bool GetGCFlag( void ) const { return CASGCBaseClass<CASRefCountedBaseClass>::GetGCFlag(); }
	void SetGCFlag( void ) const { CASGCBaseClass<CASRefCountedBaseClass>::SetGCFlag(); }

	CASVote( const CString &name, const CString &text, float length, float percentage );
	~CASVote();

	void AddRef() const;
	void Release() const;
	void EnumReferences( asIScriptEngine *engine );
	void ReleaseReferences( asIScriptEngine *engine );
	void ClearCallbacks();

	void VoteBlocked( float time );
	void VoteEnd( bool result, int voters );
	const char *GetName() const;
	float GetVoteLength() const;
	const char *GetVoteMessage() const;
	float GetPercentageNeeded() const;
	const char *GetYesText() const;
	const char *GetNoText() const;

	const CString &GetNameAS() const;
	const CString &GetVoteTextAS() const;
	const CString &GetYesTextAS() const;
	const CString &GetNoTextAS() const;
	void SetVoteText( const CString &text );
	void SetYesText( const CString &text );
	void SetNoText( const CString &text );
	void SetVoteBlockedCallback( asIScriptFunction *function );
	void SetVoteEndCallback( asIScriptFunction *function );
	void Start();
	CScriptAny *GetUserData();
	void SetUserData( CScriptAny *userData );
	void ClearUserData();

private:
	CString m_szName;
	CString m_szVoteText;
	CString m_szYesText;
	CString m_szNoText;

	float m_flVoteLength;
	float m_flPercentageNeeded;

	CASFunction *m_pVoteBlockedCallback;
	CASFunction *m_pVoteEndCallback;
	CScriptAny  *m_pUserData;
};

CASVote *CreateVote( const CString &name, const CString &text, float length, float percentage );

#endif
