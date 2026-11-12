#ifndef TRIGGER_TRACKGOAL_H
#define TRIGGER_TRACKGOAL_H

class CTriggerTrackGoal : public CBaseDelay
{
public:
	void Spawn() override;
	void Precache() override;
	void KeyValue( KeyValueData *pkvd ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;
	int ObjectCaps() override { return 0; }

private:
	string_t	m_iszPathName;
	string_t	m_iszSoundNoPath;
};

#endif
