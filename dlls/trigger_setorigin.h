#ifndef TRIGGER_SETORIGIN_H
#define TRIGGER_SETORIGIN_H

class CTriggerSetOriginManager;

class CTriggerSetOrigin : public CBaseDelay
{
public:
	CTriggerSetOrigin();

	void KeyValue( KeyValueData *pkvd ) override;
	void OnCreate( void ) override;
	void OnDestroy( void ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;
	int ObjectCaps( void ) override { return 0; }

private:
	friend class CTriggerSetOriginManager;

	enum
	{
		MAX_ENTITIES = 64
	};

	void SetupEntities( void );
	void UpdateKnownEntities( void );
	void UpdateTick( void );
	void CopyAngles( CBaseEntity *target, CBaseEntity *source, bool normalize );

	EHANDLE m_hSource;
	EHANDLE m_hEntities[MAX_ENTITIES];
	Vector  m_vecRelativeOffsets[MAX_ENTITIES];
	Vector  m_vecOffset;
	Vector  m_vecAngleOffset;

	bool m_bInvertX;
	bool m_bInvertY;
	bool m_bInvertZ;
	int  m_iKnownCount;

	EHANDLE m_hActivator;
	EHANDLE m_hCaller;

	USE_TYPE m_TriggerState;
	string_t m_iszCopyPointer;
	bool     m_bActive;
	bool     m_bSetupPending;
};

class CTriggerSetOriginManager
{
public:
	static CTriggerSetOriginManager *GetInstance( void );

	void Add( CTriggerSetOrigin *entity );
	void Remove( CTriggerSetOrigin *entity );
	void Update( void );

private:
	struct Entry
	{
		Entry() : m_pNext( NULL ) { m_hEntity = NULL; }

		EHANDLE m_hEntity;
		Entry  *m_pNext;
	};

	CTriggerSetOriginManager() : m_pFirst( NULL ) {}

	Entry *m_pFirst;
};

#endif
