#ifndef TRIGGER_CREATEENTITY_H
#define TRIGGER_CREATEENTITY_H

#include "triggers/trigger_changevalue.h"

class CTriggerCreateEntity : public CTriggerChangeValue
{
public:
	void OnCreate( void ) override;
	void Spawn( void ) override;
	void Precache( void ) override;
	void KeyValue( KeyValueData *pkvd ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;

	int ObjectCaps( void ) override
	{
		return 0;
	}

private:
	struct KeyValueLink
	{
		KeyValueData	 key;
		KeyValueLink	*next;
	};

	string_t		 m_iszCrtEntChildClass;
	string_t		 m_iszCrtEntChildName;
	string_t		 m_iszTriggerAfter;
	KeyValueLink	*m_pKeyValues;
};

#endif
