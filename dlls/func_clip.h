#ifndef FUNC_CLIP_H
#define FUNC_CLIP_H

class CFuncClip : public CBaseEntity
{
public:
	CFuncClip() : m_vecCenter( 0, 0, 0 ) {}

	void Spawn( void );
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void Touch( CBaseEntity *other );

private:
	void EnableClip( void );
	void DeActivate( void );

	bool m_bMonsterClip;
	bool m_bActive;
	Vector m_vecCenter;
};

#endif
