#ifndef TRIGGER_VOTE_H
#define TRIGGER_VOTE_H

#include "gamerules/CBaseVote.h"

class CTriggerVote : public CBaseDelay, public CBaseVote
{
public:
	CTriggerVote();
	void KeyValue( KeyValueData *data );
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void OnDestroy( void );
	void EXPORT Vote( void );

	void VoteBlocked( float time );
	void VoteEnd( bool result, int voters );
	const char *GetName() const;
	float GetVoteLength() const;
	const char *GetVoteMessage() const;
	float GetPercentageNeeded() const;
	const char *GetYesText() const;
	const char *GetNoText() const;
	void AddRef() const;
	void Release() const;

private:
	string_t	m_iszYesString;
	string_t	m_iszNoString;
};

#endif
