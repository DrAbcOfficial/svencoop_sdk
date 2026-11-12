#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "world/trains.h"
#include "scripting/api/ASBasePathTrack.h"

void PathTrackEntity_Project( CPathTrack *start, CPathTrack *end, Vector &origin, float distance, CPathTrack *track )
{
	track->Project( start, end, &origin, distance );
}

CPathTrack *PathTrackEntity_LookAhead( Vector &origin, Vector &resultOrigin, float distance, bool move, CPathTrack *track )
{
	CPathTrack *result = track->LookAhead( &origin, distance, move );

	resultOrigin = origin;
	return result;
}
