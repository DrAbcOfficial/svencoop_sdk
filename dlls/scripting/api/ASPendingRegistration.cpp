#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "triggers/trigger_setorigin.h"

void EndFrame( void )
{
	CTriggerSetOriginManager::GetInstance()->Update();
}
