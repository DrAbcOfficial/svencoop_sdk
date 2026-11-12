#ifndef TRIGGER_CONDITION_H
#define TRIGGER_CONDITION_H

class CTriggerCondition : public CBaseDelay
{
public:
	CTriggerCondition();

	void Spawn( void ) override;
	void KeyValue( KeyValueData *pkvd ) override;
	void *GetKeyValue( const char *name, entvartype_s &type ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;
	int ObjectCaps( void ) override { return 0; }
	int Save( CSave &save ) override;
	int Restore( CRestore &restore ) override;

	static TYPEDESCRIPTION m_SaveData[];

	void EXPORT CheckValue( void );

private:
	void Result( bool result );
	void CheckString( void );
	void CheckFloat( void );
	void CheckInteger( void );
	void CheckEdict( void );
	void CheckVector( void );
	void CheckBytes( int count );
	int GetByteCondition( int index );
	float MakeVecLength( Vector *value );

	string_t m_iszValueName;
	string_t m_iszCheckValue;
	int      m_iCheckType;

	string_t m_iszSourceName;
	string_t m_iszSourceKey;
	float    m_fCheckInterval;
	void     *m_pSourceValue;

	bool m_bActive;
	bool m_bUseSource;
	bool m_bIntervalSet;
	bool m_bSpecialTarget;

	int m_iLastResult;
	int m_iCheckBehaviour;
	int m_iValueIndex;

	EHANDLE m_hTarget;
	EHANDLE m_hSource;

	void         *m_pValue;
	void         **m_ppValue;
	entvartype_s m_ValueType;
	entvartype_s m_SourceType;

	EHANDLE m_hActivator;
	EHANDLE m_hCaller;
};

#endif
