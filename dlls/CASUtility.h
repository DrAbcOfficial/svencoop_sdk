#ifndef CASUTILITY_H
#define CASUTILITY_H

#include "util.h"
#include "CASServerScriptGlobal.h"
#include "CString.h"

class asIScriptObject;

struct RGBA;

class CASUtility : public CASServerScriptGlobal<CASUtility>
{
public:
	virtual ~CASUtility();

	void BloodStream( const Vector &origin, const Vector &direction, int color, int amount );
	void BloodDrips( const Vector &origin, const Vector &direction, int color, int amount );
	Vector RandomBloodVector();
	bool ShouldShowBlood( int color );
	void BloodDecalTrace( TraceResult &trace, int color );
	void DecalTrace( TraceResult &trace, int decal );
	void PlayerDecalTrace( TraceResult &trace, int playerIndex, int decal, bool custom );
	void GunshotDecalTrace( TraceResult &trace, int decal );
	void Sparks( const Vector &position );
	void Ricochet( const Vector &position, float scale );
	float WaterLevel( const Vector &position, float minimum, float maximum );
	void Bubbles( const Vector &minimum, const Vector &maximum, int count );
	void BubbleTrail( const Vector &start, const Vector &end, int count );

	void StringToVector( Vector &vector, const CString &text, char separator );
	void StringToRGBA( RGBA &color, const CString &text, char separator );
	bool IsStringInt( const CString &text );
	bool IsStringFloat( const CString &text );
	bool IsWholeNumber( float value, int &rounded );
	bool IsString3DVec( const CString &text );

	void TraceLine( const Vector &start, const Vector &end, IGNORE_MONSTERS ignoreMonsters,
	                edict_t *ignore, TraceResult &trace );
	void TraceLine( const Vector &start, const Vector &end, IGNORE_MONSTERS ignoreMonsters,
	                IGNORE_GLASS ignoreGlass, edict_t *ignore, TraceResult &trace );
	void TraceHull( const Vector &start, const Vector &end, IGNORE_MONSTERS ignoreMonsters,
	                int hull, edict_t *ignore, TraceResult &trace );
	void TraceToss( edict_t *entity, edict_t *ignore, TraceResult &trace );
	bool TraceMonsterHull( edict_t *entity, const Vector &start, const Vector &end,
	                       IGNORE_MONSTERS ignoreMonsters, edict_t *ignore, TraceResult &trace );
	void TraceModel( const Vector &start, const Vector &end, int hull, edict_t *entity, TraceResult &trace );
	CString TraceTexture( edict_t *entity, const Vector &start, const Vector &end );
	CBaseEntity *FindEntityForward( CBaseEntity *entity, float distance );
	CBaseEntity *FindEntityForward( CBaseEntity *entity );
	TraceResult GetGlobalTrace();

	bool IsPlayerInVolume( CBasePlayer *player, CBaseEntity *volume );
	bool VoteActive() const;
	int CountPlayersInBrushVolume( bool ignoreDeadPlayers, CBaseEntity *volume, int &inside,
	                               int &outside, asIScriptObject *listener ) const;
	void FindHullIntersection( const Vector &source, TraceResult &input, TraceResult &output,
	                           const Vector &minimum, const Vector &maximum, edict_t *entity, float distance );

	void GetCircularGaussianSpread( float &x, float &y ) const;
	CString BuildEntityLogString( const CString &name, const CString &userID,
	                              const CString &authID, const CString &team ) const;
	CString BuildPlayerLogString( const CString &name, const CString &userID, const CString &authID ) const;
	CString GetPlayerLog( edict_t *entity ) const;
};

#endif
