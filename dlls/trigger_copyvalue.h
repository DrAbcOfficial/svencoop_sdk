#ifndef TRIGGER_COPYVALUE_H
#define TRIGGER_COPYVALUE_H

class CTriggerCopyValue : public CBaseDelay
{
public:
	CTriggerCopyValue();

	void Spawn( void ) override;
	void KeyValue( KeyValueData *pkvd ) override;
	void *GetKeyValue( const char *name, entvartype_s &type ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;
	int ObjectCaps( void ) override { return 0; }
	int Save( CSave &save ) override;
	int Restore( CRestore &restore ) override;

	static TYPEDESCRIPTION m_SaveData[];

	void EXPORT AutoStart( void );
	void EXPORT CopyValue( void );

private:
	void Reset( void );
	void GetEntityPointers( CBaseEntity *activator, CBaseEntity *caller );
	void SpecialValueFloat( float *value, float source );
	void SpecialValueInt( int *value, int source );
	void SpecialValueByteArray( int *value, const char *source, CBaseEntity *destination );
	bool TryKeyValue( char *value );
	static int WriteFloatToString( char *buffer, int size, float value, int conversion );
	float MakeVecLength( Vector *value );

	string_t m_iszDstValueName;
	string_t m_iszSrcValueName;
	int      m_iszValueType;
	int      m_iAppendSpaces;
	int      m_trigonometricBehaviour;
	int      m_iFloatConversion;

	bool m_bActive;
	bool m_bUseKeyValue;
	int  m_iDstValueIndex;

	EHANDLE m_hDestination;
	EHANDLE m_hSource;

	entvartype_s m_DestinationType;
	entvartype_s m_SourceType;
	void         *m_pDstValue;
	void         *m_pSrcValue;

	EHANDLE      m_hActivator;
	EHANDLE      m_hCaller;
	CBaseEntity *m_pNextTarget;
};

#endif
