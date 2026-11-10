#ifndef TRIGGER_LOOKAT_H
#define TRIGGER_LOOKAT_H

class CTriggerLookat : public CBaseDelay
{
public:
	CTriggerLookat();
	~CTriggerLookat() override;

	void Spawn( void ) override;
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

	bool        m_bActive;
	float       m_flLastSearch;
	TargetNode *m_pTargets;
	EHANDLE     m_hLookTarget;
};

#endif
