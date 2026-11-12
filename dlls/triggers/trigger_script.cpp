#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "triggers/trigger_script.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASFunction.h"
#include "scripting/core/CASLog.h"
#include "server/server_support.h"
#include <stdarg.h>

#define SF_TRIGGERSCRIPT_START_ON 1
#define SF_TRIGGERSCRIPT_FORGET_ACTIVATOR 2
#define SF_TRIGGERSCRIPT_FORGET_CALLER 4
#define SF_TRIGGERSCRIPT_KEEP_ACTIVATOR 64

LINK_ENTITY_TO_CLASS( trigger_script, CTriggerScript );

const float CTriggerScript::DEFAULT_THINK_TIME = 0.1f;

CTriggerScript::CTriggerScript()
    : m_iszScriptFunctionName( 0 ), m_iMode( MODE_TRIGGER ),
      m_flThinkDelta( DEFAULT_THINK_TIME ), m_bActive( false ),
      m_iszScriptFunctionDeclaration( 0 ), m_pFunction( NULL )
{
	m_flDelay = 0;
	m_iszKillTarget = 0;
	m_hActivator = NULL;
}

void CTriggerScript::OnDestroy( void )
{
	if ( m_pFunction )
		m_pFunction->Release();

	m_pFunction = NULL;
	CBaseDelay::OnDestroy();
}

void CTriggerScript::Spawn( void )
{
	m_bActive = ( pev->spawnflags & SF_TRIGGERSCRIPT_START_ON ) != 0
		|| ( m_iMode == MODE_TRIGGER && strlen( STRING( pev->targetname ) ) < 1 );
	m_hActivator = this;
	SetThink( &CTriggerScript::InitThink );
	pev->nextthink = gpGlobals->time;
}

void CTriggerScript::Activate( void )
{
	if ( !CASServerManager::GetInstance()->m_bBuildSucceeded )
		UTIL_Remove( this );
}

void CTriggerScript::KeyValue( KeyValueData *data )
{
	if ( FStrEq( data->szKeyName, "m_iszScriptFile" ) )
	{
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "m_iszScriptFunctionName" ) )
	{
		m_iszScriptFunctionName = AllocStringLocal( data->szValue );
		data->fHandled = TRUE;
	}
	else if ( FStrEq( data->szKeyName, "m_iMode" ) )
	{
		m_iMode = (Mode_t) atoi( data->szValue );

		if ( m_iMode < MODE_TRIGGER || m_iMode > MODE_CYCLIC )
			CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "%s: invalid mode '%d' specified!\n",
			                              pev ? STRING( pev->classname ) : "", m_iMode );
	}
	else if ( FStrEq( data->szKeyName, "m_flThinkDelta" ) )
	{
		m_flThinkDelta = atof( data->szValue );

		if ( m_flThinkDelta <= 0.0f )
		{
			// This diagnostic passes the raw targetname value
			CASLog::GetInstance()->Warning( AS_LOG_DEFAULT,
			                                "%s (targetname: %s) has invalid think delta '%f', using default (%f)\n",
			                                pev ? STRING( pev->classname ) : "", pev ? STRING( pev->targetname ) : "",
			                                m_flThinkDelta, (double) DEFAULT_THINK_TIME );
			m_flThinkDelta = DEFAULT_THINK_TIME;
		}
	}
	else
	{
		CBaseDelay::KeyValue( data );
	}
}

const char *CTriggerScript::GetParametersForMode( Mode_t mode )
{
	switch ( mode )
	{
		case MODE_TRIGGER:
			return "CBaseEntity@, CBaseEntity@, USE_TYPE, float";

		case MODE_CYCLIC:
			return "CBaseEntity@";

		default:
			return NULL;
	}
}

void CTriggerScript::InitThink( void )
{
	if ( !m_iszScriptFunctionName )
	{
		CASLog::GetInstance()->Warning( AS_LOG_DEFAULT, "%s (targetname: %s): No function specified for script!\n",
		                                pev ? STRING( pev->classname ) : "", STRING( pev->targetname ) );
		return;
	}

	const char *parameters = GetParametersForMode( m_iMode );

	if ( !parameters )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "%s (targetname: %s): Invalid mode!\n",
		                              pev ? STRING( pev->classname ) : "", STRING( pev->targetname ) );
		return;
	}

	char declaration[256];

	if ( !CASBaseCallable::FormatFunction( declaration, sizeof( declaration ), "void",
	                                       STRING( m_iszScriptFunctionName ), parameters ) )
	{
		CASLog::GetInstance()->Warning( AS_LOG_DEFAULT,
		                                "%s (targetname: %s): Can't find function %s, name is too long (maximum length: %u)!",
		                                pev ? STRING( pev->classname ) : "", STRING( pev->targetname ), STRING( m_iszScriptFunctionName ),
		                                sizeof( declaration ) - CASBaseCallable::ComputeReservedSpace( "void", parameters ) );
		return;
	}

	m_iszScriptFunctionDeclaration = AllocStringLocal( declaration );

	CASModule *module = CASServerManager::GetInstance()->GetMapModule();

	if ( !module )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "%s (targetname: %s): Map module does not exist!\n",
		                              pev ? STRING( pev->classname ) : "", STRING( pev->targetname ) );
		return;
	}

	m_pFunction = module->GetFunction( declaration );

	if ( !m_pFunction )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "%s (targetname: %s): Function with signature '%s' does not exist!\n",
		                              pev ? STRING( pev->classname ) : "", STRING( pev->targetname ), declaration );
		return;
	}

	m_pFunction->AddRef();

	if ( m_iMode == MODE_CYCLIC )
	{
		SetThink( &CTriggerScript::CyclicThink );
		CyclicThink();
	}
	else if ( m_iMode == MODE_TRIGGER )
	{
		SetThink( NULL );

		// The activation call supplies no value argument
		if ( m_bActive )
			CallFunction( 0, this, this, USE_TOGGLE );
	}
}

void CTriggerScript::CyclicThink( void )
{
	if ( m_iMode != MODE_CYCLIC || !m_bActive )
		return;

	if ( !(CBaseEntity *) m_hActivator )
		m_hActivator = this;

	if ( CallFunction( 0, this ) == 1 && m_flThinkDelta > 0.0f )
		pev->nextthink = gpGlobals->time + m_flThinkDelta;
}

int CTriggerScript::CallFunction( int argumentCount, ... )
{
	if ( !m_pFunction )
		return 0;

	va_list arguments;
	bool success;

	va_start( arguments, argumentCount );
	success = m_pFunction->Call( 0, arguments );
	va_end( arguments );

	if ( success )
		return 1;

	CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "%s (targetname: %s): Failed in call to function '%s'!\n",
	                              pev ? STRING( pev->classname ) : "", STRING( pev->targetname ), STRING( m_iszScriptFunctionDeclaration ) );
	return 0;
}

void CTriggerScript::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	m_hActivator = this;

	if ( m_iMode == MODE_TRIGGER )
	{
		m_hActivator = ( pev->spawnflags & SF_TRIGGERSCRIPT_FORGET_ACTIVATOR ) ? this : activator;
		caller = ( pev->spawnflags & SF_TRIGGERSCRIPT_FORGET_CALLER ) ? this : caller;
		CallFunction( 0, (CBaseEntity *) m_hActivator, caller, useType, value );
	}
	else if ( m_iMode == MODE_CYCLIC )
	{
		m_hActivator = ( pev->spawnflags & SF_TRIGGERSCRIPT_KEEP_ACTIVATOR ) ? activator : this;

		bool wasActive = m_bActive;

		switch ( useType )
		{
			case USE_OFF:
				m_bActive = false;
				break;

			case USE_ON:
				m_bActive = true;
				break;

			case USE_TOGGLE:
				m_bActive = !m_bActive;
				break;
		}

		if ( m_bActive != wasActive )
			pev->nextthink = m_bActive ? gpGlobals->time : 0.0f;
	}

	SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );
}
