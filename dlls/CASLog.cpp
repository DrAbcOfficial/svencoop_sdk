#include "extdll.h"
#include "util.h"
#include "CLog.h"
#include "CASLog.h"
#include <math.h>

cvar_t as_log_level = { "as_log_level", "0", FCVAR_PROTECTED };
cvar_t as_script_log_max = { "as_script_log_max", "8", FCVAR_PROTECTED };

const char *LogTime(char *buffer, unsigned int size)
{
	time_t now;

	time(&now);

	return strftime(buffer, size, "%Y-%m-%d %H:%M:%S", localtime(&now)) ? buffer : "Unknown time";
}

CASLog::CASLog() : m_pLog(NULL)
{
}

CASLog::~CASLog()
{
	Shutdown();
}

CASLog *CASLog::GetInstance(void)
{
	static CASLog *pInstance;

	if (!pInstance)
		pInstance = new CASLog;

	return pInstance;
}

void CASLog::Initialize(const char *fileName)
{
	Shutdown();

	if (!fileName)
	{
		GetInstance()->Error(AS_LOG_DEFAULT, "CASLog::Initialize: null filename!\n");
		return;
	}

	const char *logsDir = CVAR_GET_STRING("logsdir");

	if (!logsDir)
	{
		GetInstance()->Error(AS_LOG_DEFAULT, "CASLog::Initialize: failed to get log directory!\n");
		return;
	}

	char directory[CLog::MAX_DIRECTORY_LENGTH];
	int length = snprintf(directory, sizeof(directory), "%s/%s", logsDir, "Angelscript");

	if (length < 0 || (unsigned int)length >= sizeof(directory))
	{
		GetInstance()->Error(AS_LOG_DEFAULT, "CASLog::Initialize: failed to format log directory path!\n");
		return;
	}

	m_pLog = new CLog(directory, fileName);
}

void CASLog::Shutdown(void)
{
	CloseLogFile();

	if (m_pLog)
	{
		delete m_pLog;
		m_pLog = NULL;
	}
}

bool CASLog::IsLogOpen(void) const
{
	return m_pLog && m_pLog->m_pFile;
}

void CASLog::OpenLogFile(void)
{
	unsigned int count = (unsigned int)(long long)fmaxf(0.0f, as_script_log_max.value);

	m_pLog->LimitToAmount(count);

	if (!count)
		return;

	if (m_pLog && m_pLog->Open())
	{
		Log(AS_LOG_DEFAULT, "Angelscript log opened\n");

		char time[CLog::MAX_TIME_LENGTH];

		Log(AS_LOG_DEFAULT, "Server started logging Angelscript on map '%s' at: %s\n",
			STRING(gpGlobals->mapname), LogTime(time, sizeof(time)));
	}
	else
	{
		GetInstance()->Error(AS_LOG_DEFAULT, "Failed to open log file!\n");
	}
}

void CASLog::CloseLogFile(void)
{
	if (IsLogOpen())
	{
		char time[CLog::MAX_TIME_LENGTH];

		Log(AS_LOG_DEFAULT, "Server stopped logging Angelscript at: %s\n", LogTime(time, sizeof(time)));
		m_pLog->Close();
	}
}

void CASLog::Log(ASLogLevel_t level, ASLogType_t type, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	VLog(level, type, format, args);
	va_end(args);
}

void CASLog::Log(ASLogLevel_t level, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	VLog(level, format, args);
	va_end(args);
}

void CASLog::VLog(ASLogLevel_t level, const char *format, va_list args)
{
	VLog(level, AS_LOG_NORMAL, format, args);
}

void CASLog::VLog(ASLogLevel_t level, ASLogType_t type, const char *format, va_list args)
{
	if (!format || !m_pLog)
		return;

	char message[CLog::MAX_MESSAGE_LENGTH];
	int length = vsnprintf(message, sizeof(message), format, args);

	if (length <= 0 || (unsigned int)length >= sizeof(message))
		return;

	const char *prefix = "";

	if (type == AS_LOG_WARNING)
		prefix = "WARNING: ";
	else if (type == AS_LOG_ERROR)
		prefix = "ERROR: ";

	if ((int)as_log_level.value >= level)
	{
		if (!m_pLog->m_pFile)
			OpenLogFile();

		if (m_pLog->m_pFile)
		{
			m_pLog->Printf(prefix);
			m_pLog->VPrintf(format, args);
		}
	}

	ALERT(at_console, "%sAngelscript: %s", prefix, message);
}

void CASLog::Warning(ASLogLevel_t level, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	VWarning(level, format, args);
	va_end(args);
}

void CASLog::VWarning(ASLogLevel_t level, const char *format, va_list args)
{
	VLog(level, AS_LOG_WARNING, format, args);
}

void CASLog::Error(ASLogLevel_t level, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	VError(level, format, args);
	va_end(args);
}

void CASLog::VError(ASLogLevel_t level, const char *format, va_list args)
{
	VLog(level, AS_LOG_ERROR, format, args);
}
