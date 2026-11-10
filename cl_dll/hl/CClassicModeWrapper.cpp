#include "../hud.h"
#include "../cl_util.h"
#include "CClassicModeWrapper.h"

bool CClassicModeWrapper::isEnabled( void )
{
	return gHUD.m_bClassicMode;
}
