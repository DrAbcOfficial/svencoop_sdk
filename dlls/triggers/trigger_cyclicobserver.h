#ifndef TRIGGER_CYCLICOBSERVER_H
#define TRIGGER_CYCLICOBSERVER_H

class CTriggerCyclicObserver : public CBaseEntity
{
public:
	CTriggerCyclicObserver();
	~CTriggerCyclicObserver() override;

	void KeyValue( KeyValueData *data ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;

private:
	int m_Mode;
};

#endif
