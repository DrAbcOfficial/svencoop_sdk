#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "scripting/api/ASNetworkMessage.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASLog.h"
#include "server/CNetworkMessages.h"
#include "CString.h"

CASNetworkMessage *CASNetworkMessage::m_pLastCreatedMessage = NULL;

CASNetworkMessage::CASNetworkMessage(int destination, int type, edict_s *edict)
	: m_bEnded(false)
{
	m_pLastCreatedMessage = this;
	MESSAGE_BEGIN(destination, type, NULL, edict);
}

CASNetworkMessage::CASNetworkMessage(int destination, int type, const Vector &origin, edict_s *edict)
	: m_bEnded(false)
{
	m_pLastCreatedMessage = this;
	MESSAGE_BEGIN(destination, type, origin, edict);
}

CASNetworkMessage::~CASNetworkMessage()
{
	if (m_pLastCreatedMessage == this)
		m_pLastCreatedMessage = NULL;
}

void CASNetworkMessage::Release(void) const
{
	delete this;
}

void CASNetworkMessage::End(void)
{
	if (!m_bEnded && m_pLastCreatedMessage == this)
	{
		m_bEnded = true;
		MESSAGE_END();
		m_pLastCreatedMessage = NULL;
	}
}

void CASNetworkMessage::WriteByte(unsigned char value)
{
	if (!m_bEnded && m_pLastCreatedMessage == this)
		WRITE_BYTE(value);
}

void CASNetworkMessage::WriteChar(const CString &text)
{
	if (!m_bEnded && m_pLastCreatedMessage == this)
		WRITE_CHAR(text[0]);
}

void CASNetworkMessage::WriteShort(unsigned short value)
{
	if (!m_bEnded && m_pLastCreatedMessage == this)
		WRITE_SHORT(value);
}

void CASNetworkMessage::WriteLong(unsigned long value)
{
	if (!m_bEnded && m_pLastCreatedMessage == this)
		WRITE_LONG(value);
}

void CASNetworkMessage::WriteAngle(float value)
{
	if (!m_bEnded && m_pLastCreatedMessage == this)
		WRITE_ANGLE(value);
}

void CASNetworkMessage::WriteCoord(float value)
{
	if (!m_bEnded && m_pLastCreatedMessage == this)
		WRITE_COORD(value);
}

void CASNetworkMessage::WriteVector(const Vector &value)
{
	if (!m_bEnded && m_pLastCreatedMessage == this)
	{
		WRITE_COORD(value.x);
		WRITE_COORD(value.y);
		WRITE_COORD(value.z);
	}
}

void CASNetworkMessage::WriteString(const CString &text)
{
	if (!m_bEnded && m_pLastCreatedMessage == this)
		WRITE_STRING(text.Data());
}

void CASNetworkMessage::WriteEntity(int index)
{
	if (!m_bEnded && m_pLastCreatedMessage == this)
		WRITE_ENTITY(index);
}

void CASNetworkMessage::WriteEntity(CBaseEntity *entity)
{
	if (!m_bEnded && m_pLastCreatedMessage == this)
	{
		if (entity)
		{
			edict_t *edict = entity->pev ? ENT(entity->pev) : NULL;

			WRITE_ENTITY(ENTINDEX(edict));
		}
		else
		{
			CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "NetworkMessage::WriteEntity: null entity pointer!\n");
		}
	}
}

void CASNetworkMessage::WriteFloat(float value)
{
	if (!m_bEnded && m_pLastCreatedMessage == this)
	{
		const unsigned char *bytes = (const unsigned char *)&value;

		WRITE_BYTE(bytes[0]);
		WRITE_BYTE(bytes[1]);
		WRITE_BYTE(bytes[2]);
		WRITE_BYTE(bytes[3]);
	}
}

CASNetworkMessage *CreateNetworkMessage(int destination, int type, edict_t *edict)
{
	return new CASNetworkMessage(destination, type, edict);
}

CASNetworkMessage *CreateNetworkMessage(int destination, int type, const Vector &origin, edict_t *edict)
{
	return new CASNetworkMessage(destination, type, origin, edict);
}

void RegisterSCScriptNetworkMessageClass(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Network message class. Used to send messages to clients.",
		"NetworkMessage", sizeof(CASNetworkMessage), asOBJ_REF | asOBJ_SCOPED);
	documentation->RegisterObjectBehaviour("Constructor", "NetworkMessage", asBEHAVE_FACTORY,
		"NetworkMessage@ NetworkMessage(NetworkMessageDest dest, NetworkMessages::NetworkMessageType type, edict_t@ pEdict = null)",
		asFUNCTIONPR(CreateNetworkMessage, (int, int, edict_t *), CASNetworkMessage *), asCALL_CDECL);
	documentation->RegisterObjectBehaviour("Constructor", "NetworkMessage", asBEHAVE_FACTORY,
		"NetworkMessage@ NetworkMessage(NetworkMessageDest dest, NetworkMessages::NetworkMessageType type, const Vector& in vecOrigin, edict_t@ pEdict = null)",
		asFUNCTIONPR(CreateNetworkMessage, (int, int, const Vector &, edict_t *), CASNetworkMessage *), asCALL_CDECL);
	documentation->RegisterObjectBehaviour("Release", "NetworkMessage", asBEHAVE_RELEASE,
		"void Release() const", asMETHOD(CASNetworkMessage, Release), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Ends the message. Must be called to send the message.", "NetworkMessage",
		"void End()", asMETHOD(CASNetworkMessage, End), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a byte to the buffer.", "NetworkMessage",
		"void WriteByte(uint8 byte)", asMETHOD(CASNetworkMessage, WriteByte), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a character to the buffer.", "NetworkMessage",
		"void WriteChar(const string& in szChar)", asMETHOD(CASNetworkMessage, WriteChar), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a short to the buffer.", "NetworkMessage",
		"void WriteShort(int16 short)", asMETHOD(CASNetworkMessage, WriteShort), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a long to the buffer.", "NetworkMessage",
		"void WriteLong(int32 long)", asMETHOD(CASNetworkMessage, WriteLong), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes an angle to the buffer.", "NetworkMessage",
		"void WriteAngle(float flValue)", asMETHOD(CASNetworkMessage, WriteAngle), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a coordinate to the buffer.", "NetworkMessage",
		"void WriteCoord(float flValue)", asMETHOD(CASNetworkMessage, WriteCoord), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a vector to the buffer.", "NetworkMessage",
		"void WriteVector(const Vector& in vecVector)", asMETHOD(CASNetworkMessage, WriteVector), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a string to the buffer.", "NetworkMessage",
		"void WriteString(const string& in szString)", asMETHOD(CASNetworkMessage, WriteString), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes an entity index to the buffer.", "NetworkMessage",
		"void WriteEntity(int iEntity)", asMETHODPR(CASNetworkMessage, WriteEntity, (int), void), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes an entity index to the buffer.", "NetworkMessage",
		"void WriteEntity(CBaseEntity@ pEntity)", asMETHODPR(CASNetworkMessage, WriteEntity, (CBaseEntity *), void), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a float to the buffer.", "NetworkMessage",
		"void WriteFloat(float flValue)", asMETHOD(CASNetworkMessage, WriteFloat), asCALL_THISCALL);
}

void RegisterSCScriptNetworkMessage(CASDocumentation *documentation)
{
	documentation->RegisterEnum("Network message destination types.", "NetworkMessageDest");
	documentation->RegisterEnumValue("unreliable to all",
		"NetworkMessageDest", "MSG_BROADCAST", MSG_BROADCAST);
	documentation->RegisterEnumValue("reliable to one (msg_entity)",
		"NetworkMessageDest", "MSG_ONE", MSG_ONE);
	documentation->RegisterEnumValue("reliable to all",
		"NetworkMessageDest", "MSG_ALL", MSG_ALL);
	documentation->RegisterEnumValue("write to the init string",
		"NetworkMessageDest", "MSG_INIT", MSG_INIT);
	documentation->RegisterEnumValue("Ents in PVS of org",
		"NetworkMessageDest", "MSG_PVS", MSG_PVS);
	documentation->RegisterEnumValue("Ents in PAS of org",
		"NetworkMessageDest", "MSG_PAS", MSG_PAS);
	documentation->RegisterEnumValue("Reliable to PVS",
		"NetworkMessageDest", "MSG_PVS_R", MSG_PVS_R);
	documentation->RegisterEnumValue("Reliable to PAS",
		"NetworkMessageDest", "MSG_PAS_R", MSG_PAS_R);
	documentation->RegisterEnumValue("Send to one client, but don't put in reliable stream, put in unreliable datagram ( could be dropped )",
		"NetworkMessageDest", "MSG_ONE_UNRELIABLE", MSG_ONE_UNRELIABLE);
	documentation->RegisterEnumValue("Sends to all spectator proxies",
		"NetworkMessageDest", "MSG_SPEC", MSG_SPEC);

	documentation->RegisterEnum("TE_EXPLOSION flags", "TE_ExplosionFlag");
	documentation->RegisterEnumValue("All flags clear makes default Half-Life explosion",
		"TE_ExplosionFlag", "TE_EXPLFLAG_NONE", TE_EXPLFLAG_NONE);
	documentation->RegisterEnumValue("Sprite will be drawn opaque (ensure that the sprite you send is a non-additive sprite)",
		"TE_ExplosionFlag", "TE_EXPLFLAG_NOADDITIVE", TE_EXPLFLAG_NOADDITIVE);
	documentation->RegisterEnumValue("Do not render dynamic lights",
		"TE_ExplosionFlag", "TE_EXPLFLAG_NODLIGHTS", TE_EXPLFLAG_NODLIGHTS);
	documentation->RegisterEnumValue("Do not play client explosion sound",
		"TE_ExplosionFlag", "TE_EXPLFLAG_NOSOUND", TE_EXPLFLAG_NOSOUND);
	documentation->RegisterEnumValue("Do not draw particles",
		"TE_ExplosionFlag", "TE_EXPLFLAG_NOPARTICLES", TE_EXPLFLAG_NOPARTICLES);

	documentation->RegisterEnum("TE_FIREFIELD flags", "TE_FireFieldFlag");
	documentation->RegisterEnumValue("All sprites will drift upwards as they animate",
		"TE_FireFieldFlag", "TEFIRE_FLAG_ALLFLOAT", TEFIRE_FLAG_ALLFLOAT);
	documentation->RegisterEnumValue("Some of the sprites will drift upwards. (50% chance)",
		"TE_FireFieldFlag", "TEFIRE_FLAG_SOMEFLOAT", TEFIRE_FLAG_SOMEFLOAT);
	documentation->RegisterEnumValue("If set, sprite plays at 15 fps, otherwise plays at whatever rate stretches the animation over the sprite's duration.",
		"TE_FireFieldFlag", "TEFIRE_FLAG_LOOP", TEFIRE_FLAG_LOOP);
	documentation->RegisterEnumValue("If set, sprite is rendered alpha blended at 50% else, opaque",
		"TE_FireFieldFlag", "TEFIRE_FLAG_ALPHA", TEFIRE_FLAG_ALPHA);
	documentation->RegisterEnumValue("If set, all fire sprites have same initial Z instead of randomly filling a cube. ",
		"TE_FireFieldFlag", "TEFIRE_FLAG_PLANAR", TEFIRE_FLAG_PLANAR);
	documentation->RegisterEnumValue("If set, sprite is rendered non-opaque with additive",
		"TE_FireFieldFlag", "TEFIRE_FLAG_ADDITIVE", TEFIRE_FLAG_ADDITIVE);

	documentation->RegisterEnum("Temporary entity types. Used in network messages of type SVC_TEMPENTITY.", "TempEntityType");
	documentation->RegisterEnumValue("Beam effect between two points",
		"TempEntityType", "TE_BEAMPOINTS", TE_BEAMPOINTS);
	documentation->RegisterEnumValue("Beam effect between point and entity",
		"TempEntityType", "TE_BEAMENTPOINT", TE_BEAMENTPOINT);
	documentation->RegisterEnumValue("Particle effect plus ricochet sound",
		"TempEntityType", "TE_GUNSHOT", TE_GUNSHOT);
	documentation->RegisterEnumValue("Additive sprite, 2 dynamic lights, flickering particles, explosion sound, move vertically 8 pps",
		"TempEntityType", "TE_EXPLOSION", TE_EXPLOSION);
	documentation->RegisterEnumValue("Quake1 \"tarbaby\" explosion with sound",
		"TempEntityType", "TE_TAREXPLOSION", TE_TAREXPLOSION);
	documentation->RegisterEnumValue("Alphablend sprite, move vertically 30 pps",
		"TempEntityType", "TE_SMOKE", TE_SMOKE);
	documentation->RegisterEnumValue("Tracer effect from point to point",
		"TempEntityType", "TE_TRACER", TE_TRACER);
	documentation->RegisterEnumValue("TE_BEAMPOINTS with simplified parameters",
		"TempEntityType", "TE_LIGHTNING", TE_LIGHTNING);
	documentation->RegisterEnumValue("Beam effect between two entities",
		"TempEntityType", "TE_BEAMENTS", TE_BEAMENTS);
	documentation->RegisterEnumValue("Spark effect",
		"TempEntityType", "TE_SPARKS", TE_SPARKS);
	documentation->RegisterEnumValue("Lava splash effect",
		"TempEntityType", "TE_LAVASPLASH", TE_LAVASPLASH);
	documentation->RegisterEnumValue("Teleport effect",
		"TempEntityType", "TE_TELEPORT", TE_TELEPORT);
	documentation->RegisterEnumValue("Explosion effect",
		"TempEntityType", "TE_EXPLOSION2", TE_EXPLOSION2);
	documentation->RegisterEnumValue("Decal from the .BSP file ",
		"TempEntityType", "TE_BSPDECAL", TE_BSPDECAL);
	documentation->RegisterEnumValue("Tracers moving toward a point",
		"TempEntityType", "TE_IMPLOSION", TE_IMPLOSION);
	documentation->RegisterEnumValue("Line of moving glow sprites with gravity, fadeout, and collisions",
		"TempEntityType", "TE_SPRITETRAIL", TE_SPRITETRAIL);
	documentation->RegisterEnumValue("Additive sprite, plays 1 cycle",
		"TempEntityType", "TE_SPRITE", TE_SPRITE);
	documentation->RegisterEnumValue("A beam with a sprite at the end",
		"TempEntityType", "TE_BEAMSPRITE", TE_BEAMSPRITE);
	documentation->RegisterEnumValue("Screen aligned beam ring, expands to max radius over lifetime",
		"TempEntityType", "TE_BEAMTORUS", TE_BEAMTORUS);
	documentation->RegisterEnumValue("Disk that expands to max radius over lifetime",
		"TempEntityType", "TE_BEAMDISK", TE_BEAMDISK);
	documentation->RegisterEnumValue("Cylinder that expands to max radius over lifetime",
		"TempEntityType", "TE_BEAMCYLINDER", TE_BEAMCYLINDER);
	documentation->RegisterEnumValue("Create a line of decaying beam segments until entity stops moving",
		"TempEntityType", "TE_BEAMFOLLOW", TE_BEAMFOLLOW);
	documentation->RegisterEnumValue("Glowing sprite",
		"TempEntityType", "TE_GLOWSPRITE", TE_GLOWSPRITE);
	documentation->RegisterEnumValue("Connect a beam ring to two entities",
		"TempEntityType", "TE_BEAMRING", TE_BEAMRING);
	documentation->RegisterEnumValue("Oriented shower of tracers",
		"TempEntityType", "TE_STREAK_SPLASH", TE_STREAK_SPLASH);
	documentation->RegisterEnumValue("Dynamic light effect",
		"TempEntityType", "TE_DLIGHT", TE_DLIGHT);
	documentation->RegisterEnumValue("Point entity light, no world effect",
		"TempEntityType", "TE_ELIGHT", TE_ELIGHT);
	documentation->RegisterEnumValue("Text message",
		"TempEntityType", "TE_TEXTMESSAGE", TE_TEXTMESSAGE);
	documentation->RegisterEnumValue("Draws a line",
		"TempEntityType", "TE_LINE", TE_LINE);
	documentation->RegisterEnumValue("Draws a box",
		"TempEntityType", "TE_BOX", TE_BOX);
	documentation->RegisterEnumValue("Kill all beams attached to entity",
		"TempEntityType", "TE_KILLBEAM", TE_KILLBEAM);
	documentation->RegisterEnumValue("Creates a large funnel",
		"TempEntityType", "TE_LARGEFUNNEL", TE_LARGEFUNNEL);
	documentation->RegisterEnumValue("Particle spray",
		"TempEntityType", "TE_BLOODSTREAM", TE_BLOODSTREAM);
	documentation->RegisterEnumValue("Line of particles every 5 units, dies in 30 seconds",
		"TempEntityType", "TE_SHOWLINE", TE_SHOWLINE);
	documentation->RegisterEnumValue("Particle spray",
		"TempEntityType", "TE_BLOOD", TE_BLOOD);
	documentation->RegisterEnumValue("Decal applied to a brush entity (not the world)",
		"TempEntityType", "TE_DECAL", TE_DECAL);
	documentation->RegisterEnumValue("Create alpha sprites inside of entity, float upwards",
		"TempEntityType", "TE_FIZZ", TE_FIZZ);
	documentation->RegisterEnumValue("Create a moving model that bounces and makes a sound when it hits",
		"TempEntityType", "TE_MODEL", TE_MODEL);
	documentation->RegisterEnumValue("Spherical shower of models, picks from sets",
		"TempEntityType", "TE_EXPLODEMODEL", TE_EXPLODEMODEL);
	documentation->RegisterEnumValue("Box of models or sprites",
		"TempEntityType", "TE_BREAKMODEL", TE_BREAKMODEL);
	documentation->RegisterEnumValue("Decal and ricochet sound",
		"TempEntityType", "TE_GUNSHOTDECAL", TE_GUNSHOTDECAL);
	documentation->RegisterEnumValue("Spray of alpha sprites",
		"TempEntityType", "TE_SPRITE_SPRAY", TE_SPRITE_SPRAY);
	documentation->RegisterEnumValue("Quick spark sprite, client ricochet sound. ",
		"TempEntityType", "TE_ARMOR_RICOCHET", TE_ARMOR_RICOCHET);
	documentation->RegisterEnumValue("Draw a player decal (spray paint)",
		"TempEntityType", "TE_PLAYERDECAL", TE_PLAYERDECAL);
	documentation->RegisterEnumValue("Create alpha sprites inside of box, float upwards",
		"TempEntityType", "TE_BUBBLES", TE_BUBBLES);
	documentation->RegisterEnumValue("Create alpha sprites along a line, float upwards",
		"TempEntityType", "TE_BUBBLETRAIL", TE_BUBBLETRAIL);
	documentation->RegisterEnumValue("Spray of opaque sprite1's that fall, single sprite2 for 1..2 secs (this is a high-priority tent)",
		"TempEntityType", "TE_BLOODSPRITE", TE_BLOODSPRITE);
	documentation->RegisterEnumValue("Decal applied to the world brush",
		"TempEntityType", "TE_WORLDDECAL", TE_WORLDDECAL);
	documentation->RegisterEnumValue("Decal (with texture index > 256) applied to world brush",
		"TempEntityType", "TE_WORLDDECALHIGH", TE_WORLDDECALHIGH);
	documentation->RegisterEnumValue("Same as TE_DECAL, but the texture index was greater than 256",
		"TempEntityType", "TE_DECALHIGH", TE_DECALHIGH);
	documentation->RegisterEnumValue("Makes a projectile (like a nail) (this is a high-priority tent)",
		"TempEntityType", "TE_PROJECTILE", TE_PROJECTILE);
	documentation->RegisterEnumValue("Throws a shower of sprites or models",
		"TempEntityType", "TE_SPRAY", TE_SPRAY);
	documentation->RegisterEnumValue("Sprites emit from a player's bounding box (ONLY use for players!)",
		"TempEntityType", "TE_PLAYERSPRITES", TE_PLAYERSPRITES);
	documentation->RegisterEnumValue("Very similar to lavasplash.",
		"TempEntityType", "TE_PARTICLEBURST", TE_PARTICLEBURST);
	documentation->RegisterEnumValue("Makes a field of fire.",
		"TempEntityType", "TE_FIREFIELD", TE_FIREFIELD);
	documentation->RegisterEnumValue("Attaches a TENT to a player (this is a high-priority tent)",
		"TempEntityType", "TE_PLAYERATTACHMENT", TE_PLAYERATTACHMENT);
	documentation->RegisterEnumValue("Will expire all TENTS attached to a player.",
		"TempEntityType", "TE_KILLPLAYERATTACHMENTS", TE_KILLPLAYERATTACHMENTS);
	documentation->RegisterEnumValue("Much more compact shotgun message",
		"TempEntityType", "TE_MULTIGUNSHOT", TE_MULTIGUNSHOT);
	documentation->RegisterEnumValue("Larger message than the standard tracer, but allows some customization.",
		"TempEntityType", "TE_USERTRACER", TE_USERTRACER);

	documentation->SetDefaultNamespace("NetworkMessages");
	documentation->RegisterEnum("Network message types", "NetworkMessageType");
	documentation->RegisterEnumValue("Creates temporary entities on the client side.",
		"NetworkMessageType", "SVC_TEMPENTITY", SVC_TEMPENTITY);
	documentation->RegisterEnumValue("Enables intermission for clients.",
		"NetworkMessageType", "SVC_INTERMISSION", SVC_INTERMISSION);
	documentation->RegisterEnumValue("Executes command on the client side.",
		"NetworkMessageType", "SVC_STUFFTEXT", 9);

	CNetworkMessages *messages = CNetworkMessages::GetInstance();
	unsigned int lastMessage = messages->m_uiLastMessage;

	for (unsigned int index = messages->m_uiFirstMessage; index < lastMessage; index++)
	{
		const UserMessageData_t *data = CNetworkMessages::GetInstance()->GetUserMessageData(index);

		if (data && data->m_pszName)
			documentation->RegisterEnumValue("User message", "NetworkMessageType", data->m_pszName, data->m_uiIndex);
	}

	documentation->SetDefaultNamespace("");
	RegisterSCScriptNetworkMessageClass(documentation);
}
