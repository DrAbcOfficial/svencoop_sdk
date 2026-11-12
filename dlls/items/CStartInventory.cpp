#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "gamerules/gamerules.h"
#include "server/CMap.h"
#include "server/CCfgFile.h"
#include "items/CStartInventory.h"

extern DLL_GLOBAL CGameRules *g_pGameRules;

namespace
{
void AppendInventoryCommand( CCfgFile *config, CCfgCommand *command )
{
	if ( !config || !command )
		return;

	if ( config->m_pCommands )
	{
		CCfgCommand *last = config->m_pCommands;

		while ( last->m_pNext )
			last = last->m_pNext;

		command->m_pPrev = last;
		command->m_pNext = NULL;
		last->m_pNext = command;
	}
	else
	{
		config->m_pCommands = command;
		command->m_pNext = NULL;
		command->m_pPrev = NULL;
	}
}
}

CStartInventory *CStartInventory::GetInstance( void )
{
	static CStartInventory *instance;

	if ( !instance )
		instance = new CStartInventory;

	return instance;
}

bool CStartInventory::IsAvailable( void ) const
{
	return g_pGameRules && g_pGameRules->IsMultiplayer();
}

bool CStartInventory::Exists( const char *entry ) const
{
	if ( !IsAvailable() )
		return false;

	CCfgFile *config = CMap::GetInstance()->m_pMapConfigFile;

	return config && config->Find( entry );
}

void CStartInventory::Set( const char *entry, unsigned long amount )
{
	if ( !IsAvailable() )
		return;

	CCfgFile *config = CMap::GetInstance()->m_pMapConfigFile;
	CCfgCommand *command = config ? config->Find( entry ) : NULL;
	char value[40];

	snprintf( value, sizeof( value ), "%d", (int) amount );

	if ( command )
	{
		command->SetArg( value, 0 );
		return;
	}

	command = new CCfgCommand;
	command->SetCommand( entry, value, "", "", "", "" );
	AppendInventoryCommand( config, command );
}

void CStartInventory::Set( const char *entry )
{
	if ( !IsAvailable() )
		return;

	CCfgFile *config = CMap::GetInstance()->m_pMapConfigFile;

	if ( !config || config->Find( entry ) )
		return;

	CCfgCommand *command = new CCfgCommand;

	command->SetCommand( entry, "", "", "", "", "" );
	AppendInventoryCommand( config, command );
}

bool CStartInventory::Add( const char *entry, unsigned long amount, bool add )
{
	if ( !IsAvailable() )
		return false;

	CCfgFile *config = CMap::GetInstance()->m_pMapConfigFile;
	CCfgCommand *command = config ? config->Find( entry ) : NULL;
	char value[40];

	if ( command )
	{
		unsigned long previous = (unsigned long) atoi( command->GetArg( 0 ) );
		unsigned long result;

		if ( add )
		{
			result = previous + amount;

			if ( result < amount )
				result = ~0UL;
		}
		else
		{
			result = previous - amount;

			if ( result > previous )
				result = 0;
		}

		snprintf( value, sizeof( value ), "%d", (int) result );
		command->SetArg( value, 0 );
		return true;
	}

	if ( !add )
		return false;

	command = new CCfgCommand;

	// SetCommand reads this buffer before the amount is formatted
	command->SetCommand( entry, value, "", "", "", "" );
	snprintf( value, sizeof( value ), "%d", (int) amount );
	AppendInventoryCommand( config, command );

	return config != NULL;
}

void CStartInventory::Remove( const char *entry )
{
	if ( !IsAvailable() )
		return;

	CCfgFile *config = CMap::GetInstance()->m_pMapConfigFile;

	if ( !config )
		return;

	CCfgCommand *command = config->Find( entry );

	if ( !command )
		return;

	if ( command->m_pPrev )
		command->m_pPrev->m_pNext = command->m_pNext;
	else
		config->m_pCommands = command->m_pNext;

	if ( command->m_pNext )
		command->m_pNext->m_pPrev = command->m_pPrev;

	command->m_pPrev = NULL;
	command->m_pNext = NULL;
}

bool CStartInventory::Restock( const char *entry, unsigned long amount )
{
	if ( !IsAvailable() )
		return false;

	CCfgFile *config = CMap::GetInstance()->m_pMapConfigFile;
	CCfgCommand *command = config ? config->Find( entry ) : NULL;
	char value[40];

	snprintf( value, sizeof( value ), "%d", (int) amount );

	if ( command )
	{
		if ( strtoul( command->GetArg( 0 ), NULL, 10 ) >= amount )
			return false;

		command->SetArg( value, 0 );
		return true;
	}

	command = new CCfgCommand;
	command->SetCommand( entry, value, "", "", "", "" );
	AppendInventoryCommand( config, command );

	return true;
}

bool CStartInventory::Limit( const char *entry, unsigned long amount )
{
	if ( !IsAvailable() )
		return false;

	CCfgFile *config = CMap::GetInstance()->m_pMapConfigFile;
	CCfgCommand *command = config ? config->Find( entry ) : NULL;
	char value[40];

	snprintf( value, sizeof( value ), "%d", (int) amount );

	if ( !command || strtoul( command->GetArg( 0 ), NULL, 10 ) <= amount )
		return false;

	command->SetArg( value, 0 );

	return true;
}

unsigned long CStartInventory::GetAmount( const char *entry ) const
{
	if ( !IsAvailable() )
		return 0;

	CCfgFile *config = CMap::GetInstance()->m_pMapConfigFile;
	CCfgCommand *command = config ? config->Find( entry ) : NULL;

	if ( !command )
		return 0;

	return strtoul( command->GetArg( 0 ), NULL, 10 );
}
