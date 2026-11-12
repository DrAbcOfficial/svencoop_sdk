#include "extdll.h"
#include "util.h"
#include "scripting/core/CASScriptDebugger.h"
#include "scripting/core/CASDebugger.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASLog.h"

CASScriptDebugger::CASScriptDebugger()
{
}

CASScriptDebugger::~CASScriptDebugger()
{
}

unsigned int CASScriptDebugger::GetCurrentStackLevel(void) const
{
	asIScriptContext *context = asGetActiveContext();

	if (context)
		return context->GetCallstackSize();

	CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
	                             "CAngelscript::GetCurrentStackLevel() called outside angelscript code!\n");
	return 0;
}

void CASScriptDebugger::OutputLocals(void)
{
	ALERT(at_console, "Angelscript: Local variables:\n");

	CASDebugger *debugger = CASServerManager::GetInstance()->m_pDebugger;

	debugger->InterpretCommand("l v", asGetActiveContext());
}

void CASScriptDebugger::OutputGlobals(void)
{
	ALERT(at_console, "Angelscript: Global variables:\n");

	CASDebugger *debugger = CASServerManager::GetInstance()->m_pDebugger;

	debugger->InterpretCommand("l g", asGetActiveContext());
}

void CASScriptDebugger::OutputMembers(void)
{
	ALERT(at_console, "Angelscript: Member variables:\n");

	CASDebugger *debugger = CASServerManager::GetInstance()->m_pDebugger;

	debugger->InterpretCommand("l m", asGetActiveContext());
}

void CASScriptDebugger::OutputStatistics(void)
{
	ALERT(at_console, "Angelscript: Statistics:\n");

	CASDebugger *debugger = CASServerManager::GetInstance()->m_pDebugger;

	debugger->InterpretCommand("l s", asGetActiveContext());
}

void RegisterSCScriptDebugger(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Debugger", "CDebugger", 0, asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectMethod("Gets the current stack level.", "CDebugger",
	                                    "uint32 GetCurrentStackLevel() const", asMETHOD(CASScriptDebugger, GetCurrentStackLevel), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Output local variables.", "CDebugger",
	                                    "void OutputLocals()", asMETHOD(CASScriptDebugger, OutputLocals), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Output global variables.", "CDebugger",
	                                    "void OutputGlobals()", asMETHOD(CASScriptDebugger, OutputGlobals), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Output member variables.", "CDebugger",
	                                    "void OutputMembers()", asMETHOD(CASScriptDebugger, OutputMembers), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Output statistics.", "CDebugger",
	                                    "void OutputStatistics()", asMETHOD(CASScriptDebugger, OutputStatistics), asCALL_THISCALL);
	documentation->RegisterGlobalProperty("Debugger instance.", " CDebugger g_Debugger", CASScriptDebugger::GetInstance());
}
