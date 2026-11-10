#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "trigger_setorigin.h"

void EndFrame( void )
{
	CTriggerSetOriginManager::GetInstance()->Update();
}
