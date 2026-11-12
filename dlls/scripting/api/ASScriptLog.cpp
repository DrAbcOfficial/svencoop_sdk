#include "scripting/api/ASScriptLog.h"
#include "scripting/api/ASString.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASLog.h"
#include "CString.h"

CASScriptLog::CASScriptLog()
{
}

CASScriptLog::~CASScriptLog()
{
}

bool CASScriptLog::PrintF(asIScriptGeneric *generic)
{
	const CString *format = (const CString *)generic->GetArgAddress(0);
	char buffer[4096];
	bool success = ASSPrintf(buffer, sizeof(buffer), format->Data(), 1, generic);

	if (success)
		CASLog::GetInstance()->Log(AS_LOG_DEFAULT, "%s", buffer);

	return success;
}

void CASScriptLog_PrintF(asIScriptGeneric *generic)
{
	CASScriptLog *log = (CASScriptLog *)generic->GetObject();

	generic->SetReturnByte(log->PrintF(generic));
}

void RegisterSCScriptScriptLog(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("This class can log data to the Angelscript log file.", "CLog", 0, asOBJ_REF | asOBJ_NOCOUNT);
	documentation->RegisterObjectMethod("Prints formatted data to the file. Uses SC printf formatting style.", "CLog",
	                                    "bool PrintF(const string& in szFormat)", asFUNCTION(CASScriptLog_PrintF), asCALL_GENERIC);
	documentation->RegisterObjectMethod("Prints formatted data to the file. Uses SC printf formatting style.", "CLog",
	                                    "bool PrintF(const string& in szFormat, ?& in)", asFUNCTION(CASScriptLog_PrintF), asCALL_GENERIC);
	documentation->RegisterObjectMethod("Prints formatted data to the file. Uses SC printf formatting style.", "CLog",
	                                    "bool PrintF(const string& in szFormat, ?& in, ?& in)", asFUNCTION(CASScriptLog_PrintF), asCALL_GENERIC);
	documentation->RegisterObjectMethod("Prints formatted data to the file. Uses SC printf formatting style.", "CLog",
	                                    "bool PrintF(const string& in szFormat, ?& in, ?& in, ?& in)", asFUNCTION(CASScriptLog_PrintF), asCALL_GENERIC);
	documentation->RegisterObjectMethod("Prints formatted data to the file. Uses SC printf formatting style.", "CLog",
	                                    "bool PrintF(const string& in szFormat, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(CASScriptLog_PrintF), asCALL_GENERIC);
	documentation->RegisterObjectMethod("Prints formatted data to the file. Uses SC printf formatting style.", "CLog",
	                                    "bool PrintF(const string& in szFormat, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(CASScriptLog_PrintF), asCALL_GENERIC);
	documentation->RegisterObjectMethod("Prints formatted data to the file. Uses SC printf formatting style.", "CLog",
	                                    "bool PrintF(const string& in szFormat, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(CASScriptLog_PrintF), asCALL_GENERIC);
	documentation->RegisterObjectMethod("Prints formatted data to the file. Uses SC printf formatting style.", "CLog",
	                                    "bool PrintF(const string& in szFormat, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(CASScriptLog_PrintF), asCALL_GENERIC);
	documentation->RegisterObjectMethod("Prints formatted data to the file. Uses SC printf formatting style.", "CLog",
	                                    "bool PrintF(const string& in szFormat, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)", asFUNCTION(CASScriptLog_PrintF), asCALL_GENERIC);
	documentation->RegisterGlobalProperty("Log global instance", "CLog g_Log", CASScriptLog::GetInstance());
}
