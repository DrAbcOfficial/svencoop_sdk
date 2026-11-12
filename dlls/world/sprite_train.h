#ifndef SPRITE_TRAIN_H
#define SPRITE_TRAIN_H

#include "world/plats.h"
#include "utils/TypedEntityHandle.h"

class CSprite;

class CSpriteTrain : public CFuncTrain
{
	friend class CRenderFxManager;

public:
	void Spawn( void );
	void Precache( void );
	void UpdateOnRemove( void );
	int Save( CSave &save );
	int Restore( CRestore &restore );

	static TYPEDESCRIPTION m_SaveData[];

private:
	CEntityHandle<CSprite> m_hSprite;
};

#endif
