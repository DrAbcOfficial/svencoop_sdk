#ifndef CBASEVOTE_H
#define CBASEVOTE_H

class CBaseVote
{
public:
	virtual void VoteBlocked( float time ) = 0;
	virtual void VoteEnd( bool result, int voters ) = 0;
	virtual const char *GetName() const = 0;
	virtual float GetVoteLength() const = 0;
	virtual const char *GetVoteMessage() const = 0;
	virtual float GetPercentageNeeded() const = 0;
	virtual const char *GetYesText() const = 0;
	virtual const char *GetNoText() const = 0;
	virtual void AddRef() const = 0;
	virtual void Release() const = 0;

protected:
	~CBaseVote() {}
};

#endif
