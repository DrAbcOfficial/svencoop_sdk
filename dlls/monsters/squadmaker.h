#ifndef SQUADMAKER_H
#define SQUADMAKER_H

class CSquadMaker : public CBaseMonster
{
public:
	CSquadMaker();

	void Spawn( void ) override;
	void Precache( void ) override;
	void KeyValue( KeyValueData *data ) override;
	void *GetKeyValue( const char *name, entvartype_s &type ) override;
	int Save( CSave &save ) override;
	int Restore( CRestore &restore ) override;
	void DeathNotice( entvars_t *child ) override;

	void EXPORT ToggleUse( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void EXPORT CyclicUse( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void EXPORT MakerThink( void );
	void EXPORT CyclicUseThink( void );
	bool MakeMonster( void );
	edict_t *SpawnActualMonster( void );

	static TYPEDESCRIPTION m_SaveData[];

private:
	string_t m_iszMonsterClassname;
	string_t m_iszChildPathName;
	int      m_cNumMonsters;
	int      m_cLiveChildren;
	int      m_iMaxLiveChildren;
	bool     m_fActive;
	bool     m_fFadeChildren;

	bool     m_bChangeRenderMode;
	string_t m_iszNewModel;
	int      m_iNewBody;
	int      m_iNewSolid;
	int      m_iNewSkin;

	string_t m_iszChildTriggerTarget;
	int      m_iChildTriggerCondition;
	bool     m_bGag;
	string_t m_iszSquadName;
	bool     m_bSquadLeader;

	string_t m_iszWeaponPlayerModel;
	string_t m_iszWeaponWorldModel;
	string_t m_iszWeaponViewModel;
	string_t m_iszChildGuardEntity;
	int      m_iFreeRoam;
	int      m_iBloodColor;
	bool     m_bOverrideClassification;
	int      m_iChildClassification;
	bool     m_bOverrideDisplayName;
	string_t m_iszChildDisplayName;

	int      m_iQueuedSpawns;
	int      m_iSpawnMode;
	string_t m_iszXenMaker;
	int      m_iOrientation;
	string_t m_iszFunctionDeclaration;
};

#endif
