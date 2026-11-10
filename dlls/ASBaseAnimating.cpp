#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "ASBaseAnimating.h"

int AnimatingEntity_LookupSequence( const CString &label, CBaseAnimating *entity )
{
	return entity->LookupSequence( label.Data() );
}

int AnimatingEntity_FindTransition( int endingSequence, int goalSequence, int &direction, CBaseAnimating *entity )
{
	return entity->FindTransition( endingSequence, goalSequence, &direction );
}

int AnimatingEntity_FindTransition( int endingSequence, int goalSequence, CBaseAnimating *entity )
{
	return entity->FindTransition( endingSequence, goalSequence, NULL );
}

bool AnimatingEntity_ExtractBbox( int sequence, Vector &mins, Vector &maxs, CBaseAnimating *entity )
{
	return entity->ExtractBbox( sequence, mins, maxs ) != 0;
}
