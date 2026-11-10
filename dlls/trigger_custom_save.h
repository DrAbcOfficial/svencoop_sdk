#ifndef TRIGGER_CUSTOM_SAVE_H
#define TRIGGER_CUSTOM_SAVE_H

class CTriggerCustomSave : public CBaseDelay
{
public:
	~CTriggerCustomSave();

	void Spawn( void );
	void KeyValue( KeyValueData *data );
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void OnCreate( void );
	void OnDestroy( void );

private:
	void SaveData( void );
	void LoadData( void );
	char *GetSaveName( void );

	char		*m_pszSaveName;
	string_t	m_iszLevelName;
	string_t	m_iszTrigger;
};

#endif
