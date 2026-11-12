#ifndef TRIGGER_HURT_REMOTE_H
#define TRIGGER_HURT_REMOTE_H

class CTriggerHurtRemote : public CBaseDelay
{
public:
	CTriggerHurtRemote();
	~CTriggerHurtRemote() override;

	void Spawn( void ) override;
	void KeyValue( KeyValueData *data ) override;
	void EXPORT Think( void ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;

private:
	struct TargetNode
	{
		TargetNode() : entity(), next( NULL ) {}

		EHANDLE     entity;
		TargetNode *next;
	};

	void FindTargets( void );

	bool     m_bActive;
	float    m_flHurtDelay;
	float    m_flLastSearch;
	float    m_flArmorDamage;
	int      m_iDamageType;
	string_t m_iszTargetClass;

	EHANDLE     m_hActivator;
	EHANDLE     m_hCaller;
	TargetNode *m_pTargets;
};

#endif
