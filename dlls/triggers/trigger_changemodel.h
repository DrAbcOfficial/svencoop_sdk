#ifndef TRIGGER_CHANGEMODEL_H
#define TRIGGER_CHANGEMODEL_H

class CTriggerChangeModel : public CBaseDelay
{
public:
	void Spawn( void ) override;
	void Precache( void ) override;
	void KeyValue( KeyValueData *pkvd ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;
	void OnCreate( void ) override;

private:
	void ChangeModel( CBaseEntity *entity );

	int m_iBodygroup;
	int m_iValue;
};

#endif
