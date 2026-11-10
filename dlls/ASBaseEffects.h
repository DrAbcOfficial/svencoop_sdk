#ifndef ASBASEEFFECTS_H
#define ASBASEEFFECTS_H

#include "ASBaseEntity.h"
#include "effects.h"

void RegisterSCScriptBeamType( CASDocumentation *documentation );
void RegisterSCScriptBeamFlags( CASDocumentation *documentation );
void RegisterSCScriptBeamSpawnflags( CASDocumentation *documentation );
void RegisterSCScriptBeamEntityDependencies( CASDocumentation *documentation );

void SpriteEntity_SpriteInit( const CString &spriteName, const Vector &origin, CSprite *sprite );

template <class T>
void BeamEntity_SetStartEntity( CBaseEntity *start, T *beam )
{
	if ( !start )
		return;

	beam->SetStartEntity( ENTINDEX( start->pev ? ENT( start->pev ) : NULL ) );
}

template <class T>
void BeamEntity_SetEndEntity( CBaseEntity *end, T *beam )
{
	if ( !end )
		return;

	beam->SetEndEntity( ENTINDEX( end->pev ? ENT( end->pev ) : NULL ) );
}

template <class T>
void BeamEntity_PointEntInit( const Vector &start, CBaseEntity *end, T *beam )
{
	if ( !end )
		return;

	beam->PointEntInit( start, ENTINDEX( end->pev ? ENT( end->pev ) : NULL ) );
}

template <class T>
void BeamEntity_EntsInit( CBaseEntity *start, CBaseEntity *end, T *beam )
{
	if ( !start || !end )
		return;

	int endIndex = ENTINDEX( end->pev ? ENT( end->pev ) : NULL );
	int startIndex = ENTINDEX( start->pev ? ENT( start->pev ) : NULL );

	beam->EntsInit( startIndex, endIndex );
}

template <class T>
void BeamEntity_BeamInit( const CString &spriteName, int width, T *beam )
{
	const char *name = STRING( AllocStringLocal( spriteName.Data() ) );

	beam->BeamInit( name, width );
}

template <class T>
void RegisterSCScriptBaseBeamEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CBeam, T>( documentation, "CBeam", name );
	RegisterSCScriptBaseEntity<T>( documentation, name, true );

	documentation->RegisterObjectMethod(
	    "Sets the type of the beam.", name,
	    "void SetType(BeamType type)",
	    asMETHOD( T, SetType ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the flags of the beam. Must be a combination of BeamFlags values, or 0.", name,
	    "void SetFlags(int flags)",
	    asMETHOD( T, SetFlags ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the starting position of the beam. Must be a beam of type BEAM_POINTS, BEAM_ENTPOINT or BEAM_HOSE.", name,
	    "void SetStartPos(const Vector& in vecPos)",
	    asMETHOD( T, SetStartPos ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the ending position of the beam. Must be a beam of type BEAM_POINTS or BEAM_HOSE.", name,
	    "void SetEndPos(const Vector& in vecPos)",
	    asMETHOD( T, SetEndPos ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the starting entity of the beam. Must be a beam of type BEAM_ENTS.", name,
	    "void SetStartEntity(int entityIndex)",
	    asMETHOD( T, SetStartEntity ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the starting entity of the beam. Must be a beam of type BEAM_ENTS.", name,
	    "void SetStartEntity(CBaseEntity@ pEntity)",
	    asFUNCTION( BeamEntity_SetStartEntity<T> ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Sets the ending entity of the beam. Must be a beam of type BEAM_ENTS or BEAM_ENTPOINT.", name,
	    "void SetEndEntity(int entityIndex)",
	    asMETHOD( T, SetEndEntity ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the ending entity of the beam. Must be a beam of type BEAM_ENTS or BEAM_ENTPOINT.", name,
	    "void SetEndEntity(CBaseEntity@ pEntity)",
	    asFUNCTION( BeamEntity_SetEndEntity<T> ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Sets the start attachment of the beam. Attachment is a point on the entity's model. Must be a beam of "
	    "type BEAM_ENTS. Set to 0 to disable.",
	    name,
	    "void SetStartAttachment(int attachment)",
	    asMETHOD( T, SetStartAttachment ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the end attachment of the beam. Attachment is a point on the entity's model. Must be a beam of type "
	    "BEAM_ENTS or BEAM_ENTPOINT. Set to 0 to disable.",
	    name,
	    "void SetEndAttachment(int attachment)",
	    asMETHOD( T, SetEndAttachment ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the sprite to use. The index is the one returned from CGame::PrecacheModel.", name,
	    "void SetTexture(int spriteIndex)",
	    asMETHOD( T, SetTexture ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the width of the beam.", name,
	    "void SetWidth(int iWidth)",
	    asMETHOD( T, SetWidth ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the noise (amplitude) of the beam.", name,
	    "void SetNoise(int iAmplitude)",
	    asMETHOD( T, SetNoise ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the color of the beam.", name,
	    "void SetColor(int r, int g, int b)",
	    asMETHOD( T, SetColor ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the brightness of the beam. Value is between 0-255.", name,
	    "void SetBrightness(int brightness)",
	    asMETHOD( T, SetBrightness ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the frame number of the beam.", name,
	    "void SetFrame(float frame)",
	    asMETHOD( T, SetFrame ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the scroll rate of the beam.", name,
	    "void SetScrollRate(int speed)",
	    asMETHOD( T, SetScrollRate ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the beam's starting position.", name,
	    "const Vector& GetStartPos()",
	    asMETHOD( T, GetStartPos ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the beam's ending position.", name,
	    "const Vector& GetEndPos()",
	    asMETHOD( T, GetEndPos ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the beam's texture (sprite) index.", name,
	    "int GetTexture()",
	    asMETHOD( T, GetTexture ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the beam's width.", name,
	    "int GetWidth()",
	    asMETHOD( T, GetWidth ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the beam's noise.", name,
	    "int GetNoise()",
	    asMETHOD( T, GetNoise ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the beam's color.", name,
	    "void GetColor(int& out r, int& out g, int& out b)",
	    asMETHOD( T, GetColor ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the beam's brightness.", name,
	    "int GetBrightness()",
	    asMETHOD( T, GetBrightness ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the beam's frame number.", name,
	    "int GetFrame()",
	    asMETHOD( T, GetFrame ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the beam's scroll rate.", name,
	    "int GetScrollRate()",
	    asMETHOD( T, GetScrollRate ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Call after you change start/end positions.", name,
	    "void RelinkBeam()",
	    asMETHOD( T, RelinkBeam ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates sparks at the given start and end points, if appropriate spawn flags are set.", name,
	    "void DoSparks(const Vector& in vecStart, const Vector& in vecEnd)",
	    asMETHOD( T, DoSparks ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Damages the entity hit by the given trace line with the amount set in this entity's pev->dmg "
	    "variable.\nAlso applies decals to BSP models if the appropriate spawnflag is set.",
	    name,
	    "void BeamDamage(TraceResult& in tr)",
	    asMETHOD( T, BeamDamage ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Initializes this beam with default values and sets the given sprite as the sprite used, with the given "
	    "width.",
	    name,
	    "void BeamInit(const string& in szSpriteName, int iWidth)",
	    asFUNCTION( BeamEntity_BeamInit<T> ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Initializes this beam to be a beam between 2 given points (BEAM_POINTS).", name,
	    "void PointsInit(const Vector& in vecStart, const Vector& in vecEnd)",
	    asMETHOD( T, PointsInit ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Initializes this beam to be a beam between a point and an entity (BEAM_ENTPOINT).", name,
	    "void PointEntInit(const Vector& in vecStart, int endIndex)",
	    asMETHOD( T, PointEntInit ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Initializes this beam to be a beam between a point and an entity (BEAM_ENTPOINT).", name,
	    "void PointEntInit(const Vector& in vecStart, CBaseEntity@ pEntEnd)",
	    asFUNCTION( BeamEntity_PointEntInit<T> ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Initializes this beam to be a beam between 2 given entities (BEAM_ENTS).", name,
	    "void EntsInit(int startIndex, int endIndex)",
	    asMETHOD( T, EntsInit ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Initializes this beam to be a beam between 2 given entities (BEAM_ENTS).", name,
	    "void EntsInit(CBaseEntity@ pEntStart, CBaseEntity@ pEntEnd)",
	    asFUNCTION( BeamEntity_EntsInit<T> ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Initializes this beam to be a beam going from a point in a given direction (BEAM_HOSE).", name,
	    "void HoseInit(const Vector& in vecStart, const Vector& in vecDirection)",
	    asMETHOD( T, HoseInit ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets this beam to live for the given amount of time, after which it is removed.", name,
	    "void LiveForTime(float flTime)",
	    asMETHOD( T, LiveForTime ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Causes the given amount of damage to be done to the entity hit by the given traceresult, as if the damage "
	    "occured one second ago.",
	    name,
	    "void BeamDamageInstant(TraceResult& in tr, float flDamage)",
	    asMETHOD( T, BeamDamageInstant ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates a Xen teleport zap effect between the given points.", name,
	    "void XenTeleportZap(const Vector& in vecSrc, const Vector& in vecDest)",
	    asMETHOD( T, XenTeleportZap ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Creates a Xen teleport effect at the given location, with zaps occuring within the given radius.", name,
	    "void XenTeleport(const Vector& in vecSrc, int iRadius = 256)",
	    asMETHOD( T, XenTeleport ), asCALL_THISCALL );
}

template <class T>
void RegisterSCScriptBaseLaserEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CLaser, T>( documentation, "CLaser", name );
	RegisterSCScriptBaseBeamEntity<T>( documentation, name );

	documentation->RegisterObjectMethod(
	    "Turns the laser on", name,
	    "void TurnOn()",
	    asMETHOD( T, TurnOn ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Turns the laser off", name,
	    "void TurnOff()",
	    asMETHOD( T, TurnOff ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Returns whether the laser is on", name,
	    "bool IsOn()",
	    asMETHOD( T, IsOn ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Fires the laser at a point defined by where the TraceResult hit.\nAlso causes beam damage as if "
	    "CBeam::BeamDamage were used, and sparks at both ends, if the approprite spawn flags have been set.",
	    name,
	    "void FireAtPoint(TraceResult& in tr)",
	    asMETHOD( T, FireAtPoint ), asCALL_THISCALL );
}

template <class T>
void RegisterSCScriptBaseSpriteEntity( CASDocumentation *documentation, const char *name )
{
	RegisterASScriptEntityHeaderCasts<CSprite, T>( documentation, "CSprite", name );
	RegisterSCScriptBaseEntity<T>( documentation, name, true );

	documentation->RegisterObjectMethod(
	    "Advances this sprite's frame by the given amount of frames.", name,
	    "void Animate(float flFrames)",
	    asMETHOD( T, Animate ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Causes this sprite to scale up and fade at the given rates.\nThis sprite will be removed once faded.", name,
	    "void Expand(float flScaleSpeed, float flFadeSpeed)",
	    asMETHOD( T, Expand ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Initializes this sprite.", name,
	    "void SpriteInit(const string& in szSpriteName, const Vector& in vecOrigin)",
	    asFUNCTION( SpriteEntity_SpriteInit ), asCALL_CDECL_OBJLAST );

	documentation->RegisterObjectMethod(
	    "Makes this sprite follow the given entity.", name,
	    "void SetAttachment(edict_t@ pEntity, int iAttachment)",
	    asMETHOD( T, SetAttachment ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Turns off this sprite.", name,
	    "void TurnOff()",
	    asMETHOD( T, TurnOff ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Turns on this sprite.", name,
	    "void TurnOn()",
	    asMETHOD( T, TurnOn ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Gets the number of frames in this sprite.", name,
	    "int Frames() const",
	    asMETHODPR( T, Frames, () const, int ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the transparency of this sprite.", name,
	    "void SetTransparency(int renderMode, int r, int g, int b, int renderAmount, int renderFx)",
	    asMETHOD( T, SetTransparency ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the texture used by this sprite. Is a model index returned by g_Game.PrecacheModel.", name,
	    "void SetTexture(int iSpriteIndex)",
	    asMETHOD( T, SetTexture ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the current scale.", name,
	    "void SetScale(float flScale)",
	    asMETHOD( T, SetScale ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the color.", name,
	    "void SetColor(int r, int g, int b)",
	    asMETHOD( T, SetColor ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Sets the brightness (render amount).", name,
	    "void SetBrightness(int brightness)",
	    asMETHOD( T, SetBrightness ), asCALL_THISCALL );

	documentation->RegisterObjectMethod(
	    "Makes this sprite play until time pev->dmgtime has been passed, then removes itself.", name,
	    "void AnimateAndDie(float flFramerate)",
	    asMETHOD( T, AnimateAndDie ), asCALL_THISCALL );
}

#endif
