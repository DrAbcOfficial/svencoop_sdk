#ifndef TRIGGER_SCRIPT_H
#define TRIGGER_SCRIPT_H

#include "utils/TypedEntityHandle.h"

class CASFunction;

class CTriggerScript : public CBaseDelay
{
public:
	enum Mode_t
	{
		MODE_TRIGGER = 1,
		MODE_CYCLIC = 2
	};

	CTriggerScript();
	void OnDestroy( void );
	void Spawn( void );
	void Activate( void );
	void KeyValue( KeyValueData *data );
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void EXPORT InitThink( void );
	void EXPORT CyclicThink( void );
	int CallFunction( int argumentCount, ... );
	static const char *GetParametersForMode( Mode_t mode );

private:
	static const float DEFAULT_THINK_TIME;

	CEntityHandle<CBaseEntity> m_hActivator;

	string_t     m_iszScriptFunctionName;
	Mode_t       m_iMode;
	float        m_flThinkDelta;
	bool         m_bActive;
	string_t     m_iszScriptFunctionDeclaration;
	CASFunction *m_pFunction;
};

#endif
