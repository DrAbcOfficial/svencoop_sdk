#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"
#include "scripted.h"
#include "ASBaseCineMonster.h"
#include "server_support.h"

bool CineMonsterEntity_StartSequence( CBaseMonster *target, const CString &sequence, bool completeOnEmpty, CCineMonster *cine )
{
	return cine->StartSequence( target, AllocStringLocal( sequence.Data() ), completeOnEmpty ) != FALSE;
}
