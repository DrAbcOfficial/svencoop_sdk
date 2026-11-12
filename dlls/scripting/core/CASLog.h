#ifndef CASLOG_H
#define CASLOG_H

#include <stdarg.h>

class CLog;
struct cvar_s;

enum ASLogLevel_t
{
	AS_LOG_DEFAULT = 0
};

class CASLog
{
public:
	enum { MAX_LOG_FILES = 8 };

	enum ASLogType_t
	{
		AS_LOG_NORMAL,
		AS_LOG_WARNING,
		AS_LOG_ERROR
	};

	CASLog();
	~CASLog();

	static CASLog *GetInstance(void);
	void Initialize(const char *fileName);
	void Shutdown(void);
	bool IsLogOpen(void) const;
	void OpenLogFile(void);
	void CloseLogFile(void);
	void Log(ASLogLevel_t level, ASLogType_t type, const char *format, ...);
	void Log(ASLogLevel_t level, const char *format, ...);
	void VLog(ASLogLevel_t level, ASLogType_t type, const char *format, va_list args);
	void VLog(ASLogLevel_t level, const char *format, va_list args);
	void Warning(ASLogLevel_t level, const char *format, ...);
	void VWarning(ASLogLevel_t level, const char *format, va_list args);
	void Error(ASLogLevel_t level, const char *format, ...);
	void VError(ASLogLevel_t level, const char *format, va_list args);

private:
	CLog	*m_pLog;
};

extern cvar_s as_log_level;
extern cvar_s as_script_log_max;

#endif
