#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "effects.h"
#include "ASBaseEffects.h"
#include "server_support.h"

void SpriteEntity_SpriteInit( const CString &spriteName, const Vector &origin, CSprite *sprite )
{
	const char *name = STRING( AllocStringLocal( spriteName.Data() ) );

	sprite->SpriteInit( name, origin );
}

void RegisterSCScriptBeamType( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Beam types used by CBeam::SetType", "BeamType", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue(
	    "Beam is a beam between 2 points",
	    "BeamType", "BEAM_POINTS", 0 );
	documentation->RegisterEnumValue(
	    "Beam is a beam between point and entity",
	    "BeamType", "BEAM_ENTPOINT", 1 );
	documentation->RegisterEnumValue(
	    "Beam is a beam between entities",
	    "BeamType", "BEAM_ENTS", 2 );
	documentation->RegisterEnumValue(
	    "I don't even know what this does",
	    "BeamType", "BEAM_HOSE", 3 );
}

void RegisterSCScriptBeamFlags( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "Beam flags used by CBeam::SetFlags", "BeamFlags", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue(
	    "Beam shape follows a sinus shape",
	    "BeamFlags", "BEAM_FSINE", 16 );
	documentation->RegisterEnumValue(
	    "Beam is a solid shape",
	    "BeamFlags", "BEAM_FSOLID", 32 );
	documentation->RegisterEnumValue(
	    "Beam fades in at its start point",
	    "BeamFlags", "BEAM_FSHADEIN", 64 );
	documentation->RegisterEnumValue(
	    "Beam fades out at its end point",
	    "BeamFlags", "BEAM_FSHADEOUT", 128 );
}

void RegisterSCScriptBeamSpawnflags( CASDocumentation *documentation )
{
	documentation->RegisterEnum( "CBeam spawn flags", "CBeamSpawnflags", CASDocumentation::ENUM_SIGNED );
	documentation->RegisterEnumValue(
	    "Whether the beam should start on",
	    "CBeamSpawnflags", "SF_BEAM_STARTON", 1 );
	documentation->RegisterEnumValue(
	    "Whether the beam should be togglable by triggering it",
	    "CBeamSpawnflags", "SF_BEAM_TOGGLE", 2 );
	documentation->RegisterEnumValue(
	    "Whether beams are created to a randomly selected entity with the given target name",
	    "CBeamSpawnflags", "SF_BEAM_RANDOM", 4 );
	documentation->RegisterEnumValue(
	    "Whether this beam is a ring between 2 points",
	    "CBeamSpawnflags", "SF_BEAM_RING", 8 );
	documentation->RegisterEnumValue(
	    "Whether to allow creation of sparks at the beam's start point",
	    "CBeamSpawnflags", "SF_BEAM_SPARKSTART", 16 );
	documentation->RegisterEnumValue(
	    "Whether to allow creation of sparks at the beam's end point",
	    "CBeamSpawnflags", "SF_BEAM_SPARKEND", 32 );
	documentation->RegisterEnumValue(
	    "Whether to create decals when the beam does damage to BSP models",
	    "CBeamSpawnflags", "SF_BEAM_DECALS", 64 );
	documentation->RegisterEnumValue(
	    "Whether to fade the beam in at its start point",
	    "CBeamSpawnflags", "SF_BEAM_SHADEIN", 128 );
	documentation->RegisterEnumValue(
	    "Whether to fade the beam out at its end point",
	    "CBeamSpawnflags", "SF_BEAM_SHADEOUT", 256 );
	documentation->RegisterEnumValue(
	    "Whether the beam is a temporary entity",
	    "CBeamSpawnflags", "SF_BEAM_TEMPORARY", 32768 );
}

void RegisterSCScriptBeamEntityDependencies( CASDocumentation *documentation )
{
	RegisterSCScriptBeamType( documentation );
	RegisterSCScriptBeamFlags( documentation );
	RegisterSCScriptBeamSpawnflags( documentation );
}
