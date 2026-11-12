#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "ai/basemonster.h"
#include "ai/scripted.h"
#include "scripting/api/ASBaseCineMonster.h"
#include "server/server_support.h"

bool CineMonsterEntity_StartSequence( CBaseMonster *target, const CString &sequence, bool completeOnEmpty, CCineMonster *cine )
{
	return cine->StartSequence( target, AllocStringLocal( sequence.Data() ), completeOnEmpty ) != FALSE;
}
