#include "extdll.h"
#include "util.h"
#include "scripting/api/ASEngine.h"
#include "scripting/core/CASDocumentation.h"

static const unsigned int SCRIPT_FTRACE_SIMPLEBOX = FTRACE_SIMPLEBOX;

void RegisterSCScriptEngine(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Engine global variables", "CEngine", 0, asOBJ_REF | asOBJ_NOCOUNT);

	documentation->RegisterObjectProperty("Current time", "CEngine",
		"const float time", asOFFSET(globalvars_t, time));
	documentation->RegisterObjectProperty("Time between frames", "CEngine",
		"const float frametime", asOFFSET(globalvars_t, frametime));
	documentation->RegisterObjectProperty("How many times to force all entities to retouch entities they are currently touching/intersecting. Should always be >= 0.", "CEngine",
		"float force_retouch", asOFFSET(globalvars_t, force_retouch));
	documentation->RegisterObjectProperty("Name of the current map", "CEngine",
		"const string_t mapname", asOFFSET(globalvars_t, mapname));
	documentation->RegisterObjectProperty("Start location in the map", "CEngine",
		"const string_t startspot", asOFFSET(globalvars_t, startspot));
	documentation->RegisterObjectProperty("Flag telling whether this is deathmatch", "CEngine",
		"float deathmatch", asOFFSET(globalvars_t, deathmatch));
	documentation->RegisterObjectProperty("Flag telling whether this is coop", "CEngine",
		"float coop", asOFFSET(globalvars_t, coop));
	documentation->RegisterObjectProperty("Flag telling whether this is teamplay", "CEngine",
		"float teamplay", asOFFSET(globalvars_t, teamplay));
	documentation->RegisterObjectProperty("Server flags", "CEngine",
		"const float serverflags", asOFFSET(globalvars_t, serverflags));
	documentation->RegisterObjectProperty("Found secrets", "CEngine",
		"float found_secrets", asOFFSET(globalvars_t, found_secrets));
	documentation->RegisterObjectProperty("Forward vector", "CEngine",
		"const Vector v_forward", asOFFSET(globalvars_t, v_forward));
	documentation->RegisterObjectProperty("Up vector", "CEngine",
		"const Vector v_up", asOFFSET(globalvars_t, v_up));
	documentation->RegisterObjectProperty("Right vector", "CEngine",
		"const Vector v_right", asOFFSET(globalvars_t, v_right));
	documentation->RegisterObjectProperty("Trace all is solid flag", "CEngine",
		"const float trace_allsolid", asOFFSET(globalvars_t, trace_allsolid));
	documentation->RegisterObjectProperty("Trace start is solid flag", "CEngine",
		"const float trace_startsolid", asOFFSET(globalvars_t, trace_startsolid));
	documentation->RegisterObjectProperty("Trace fraction", "CEngine",
		"const float trace_fraction", asOFFSET(globalvars_t, trace_fraction));
	documentation->RegisterObjectProperty("Trace end position", "CEngine",
		"const Vector trace_endpos", asOFFSET(globalvars_t, trace_endpos));
	documentation->RegisterObjectProperty("Trace plane normal", "CEngine",
		"const Vector trace_plane_normal", asOFFSET(globalvars_t, trace_plane_normal));
	documentation->RegisterObjectProperty("Trace plane distance", "CEngine",
		"const float trace_plane_dist", asOFFSET(globalvars_t, trace_plane_dist));
	documentation->RegisterObjectProperty("Trace entity", "CEngine",
		"const edict_t@ trace_ent", asOFFSET(globalvars_t, trace_ent));
	documentation->RegisterObjectProperty("Trace is in open flag", "CEngine",
		"const float trace_inopen", asOFFSET(globalvars_t, trace_inopen));
	documentation->RegisterObjectProperty("Trace is in water flag", "CEngine",
		"const float trace_inwater", asOFFSET(globalvars_t, trace_inwater));
	documentation->RegisterObjectProperty("Trace hit group", "CEngine",
		"const int trace_hitgroup", asOFFSET(globalvars_t, trace_hitgroup));
	documentation->RegisterObjectProperty("Trace flags", "CEngine",
		"const int trace_flags", asOFFSET(globalvars_t, trace_flags));
	documentation->RegisterObjectProperty("Message entity", "CEngine",
		"const int msg_entity", asOFFSET(globalvars_t, msg_entity));
	documentation->RegisterObjectProperty("CD audio track", "CEngine",
		"int cdAudioTrack", asOFFSET(globalvars_t, cdAudioTrack));
	documentation->RegisterObjectProperty("Maximum connected clients", "CEngine",
		"const int maxClients", asOFFSET(globalvars_t, maxClients));
	documentation->RegisterObjectProperty("Maximum entities", "CEngine",
		"const int maxEntities", asOFFSET(globalvars_t, maxEntities));

	documentation->RegisterGlobalProperty("Engine global variables instance", "CEngine g_Engine", gpGlobals);
	documentation->RegisterGlobalProperty("Goes into g_Engine.trace_flags\nTraceline with a simple box",
		"const uint FTRACE_SIMPLEBOX", (void *)&SCRIPT_FTRACE_SIMPLEBOX);
}
