#ifndef GLOBAL_LIGHT_CONTROL_H
#define GLOBAL_LIGHT_CONTROL_H

class CGlobalLightControl : public CPointEntity
{
public:
	void Spawn( void ) override;
	void KeyValue( KeyValueData *pkvd ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;
	int Save( CSave &save ) override;
	int Restore( CRestore &restore ) override;

	static TYPEDESCRIPTION m_SaveData[];

private:
	string_t	m_iszPattern;
	string_t	m_iszTargetEntity;
};

#endif
