#ifndef TRIGGER_ENTITY_ITERATOR_H
#define TRIGGER_ENTITY_ITERATOR_H

#include <vector>

class CTriggerEntityIterator : public CBaseEntity
{
public:
	void Spawn( void ) override;
	void KeyValue( KeyValueData *pkvd ) override;
	void *GetKeyValue( const char *name, entvartype_s &type ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;
	void OnDestroy( void ) override;
	int ObjectCaps( void ) override { return 0; }

	CTriggerEntityIterator *MakeCloneForThreading( void );
	void EXPORT ProcessIteration( void );

private:
	void Stop( void );
	void ClearEntityList( void );
	void AddEntity( CBaseEntity *entity );
	bool BuildEntityList( void );
	void IterateAll( void );
	void TriggerEntity( CBaseEntity *entity );
	void FinishRun( bool triggerAfterRun );
	const char *NameFilter( void );
	const char *ClassnameFilter( void );
	bool HasFilters( void );
	CBaseEntity *FindNext( bool useName, bool useClassname );

	USE_TYPE	m_triggerState;
	int			m_iStatusFilter;
	int			m_iRunMode;
	bool		m_bActive;
	float		m_flDelayBetweenTriggers;
	float		m_flDelayBetweenRuns;
	string_t	m_iszClassnameFilter;
	string_t	m_iszTriggerAfterRun;
	string_t	m_iszNameFilter;
	int			m_iMaximumRuns;

	std::vector<EHANDLE>	m_Entities;
	EHANDLE		m_hTarget;
	EHANDLE		m_hActivator;
	int			m_iCurrentEntity;
	int			m_iRunCount;
	CBaseEntity	*m_pSearchCursor;
};

#endif
