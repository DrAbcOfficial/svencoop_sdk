#include "CASScriptEntityPreprocessor.h"
#include "CKeyvalues.h"
#include "CASServerManager.h"
#include "CASModuleHandle.h"
#include "CASModuleBuilder.h"
#include "CASLog.h"

static const ASLogLevel_t SCRIPT_ENTITY_LOG_LEVEL = (ASLogLevel_t) 2;

CASScriptEntityPreprocessor::~CASScriptEntityPreprocessor()
{
}

bool CASScriptEntityPreprocessor::ShouldPreprocess( const char *className ) const
{
	return true;
}

void CASScriptEntityPreprocessor::PreprocessEntity( const CString &className, const CKvBlockNode &entity )
{
	CString scriptFile = entity.FindFirstKeyvalue( "m_iszScriptFile" );

	if ( scriptFile.IsEmpty() )
		return;

	CASModuleHandle *module = CASServerManager::GetInstance()->m_pMapModule;

	if ( module )
	{
		int result = module->m_pBuilder->AddScriptFromFile( scriptFile.Data() );

		if ( result == 0 )
		{
			CString targetName = entity.FindFirstKeyvalue( "targetname" );

			CASLog::GetInstance()->Log( SCRIPT_ENTITY_LOG_LEVEL,
			                            "%s(targetname: %s): Script %s loaded\n",
			                            className.Data(), targetName.Data(), scriptFile.Data() );
		}
	}
	else
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT,
		                              "%s: map module does not exist!\n", className.Data() );
	}
}

void CASScriptEntityPreprocessor::PreprocessStateChanged( PreprocessState state )
{
	if ( state == PREPROCESS_BEGIN )
	{
		CASLog::GetInstance()->Log( SCRIPT_ENTITY_LOG_LEVEL,
		                            "Beginning scan for m_iszScriptFile keyvalues\n" );
	}
	else if ( state == PREPROCESS_END )
	{
		CASLog::GetInstance()->Log( SCRIPT_ENTITY_LOG_LEVEL,
		                            "Finished scanning for m_iszScriptFile keyvalues\n" );

		CASServerManager::GetInstance()->LoadModules();
	}
}
