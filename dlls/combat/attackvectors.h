#ifndef ATTACKVECTORS_H
#define ATTACKVECTORS_H

class CBaseEntity;

struct AttackVector
{
	Vector			m_vecPoint;
	AttackVector	*m_pNext;
};

class CAttackVectors
{
public:
	CAttackVectors();
	~CAttackVectors();

	void Add( const Vector &point );
	void Clear( void );
	bool PointIsOnPlane( CBaseEntity *entity, const Vector &point );
	void DrawSprite( CBaseEntity *entity, const Vector &point, bool onPlane );
	void CalculateAttackVectors( CBaseEntity *entity );

	AttackVector *m_pHead;
};

#endif
